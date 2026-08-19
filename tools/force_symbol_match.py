from __future__ import annotations
import argparse
from pathlib import Path

def compare_funcs_line_by_line(control_lines:list[str], test_lines:list[str], new_links:dict[str, str]):
    control_func_name = control_lines[0].removeprefix(".fn ").split(",")[0]
    test_func_name = test_lines[0].removeprefix(".fn ").split(",")[0]

    if test_func_name != control_func_name:
        new_links.setdefault(test_func_name, control_func_name)

    for control_line, test_line in zip(control_lines, test_lines):
        if test_line.endswith("@ha") or test_line.endswith("@l"):
            test_symbol = test_line.split(" ")[-1].removesuffix("@ha").removesuffix("@l").strip("\"")
            control_symbol = control_line.split(" ")[-1].removesuffix("@ha").removesuffix("@l").strip("\"")
            if test_symbol != control_symbol:
                new_links.setdefault(test_symbol, control_symbol)
        elif "bl " in control_line:
            test_symbol = test_line.split("bl ")[-1]
            control_symbol = control_line.split("bl ")[-1]
            if test_symbol != control_symbol:
                new_links.setdefault(test_symbol, control_symbol)


def Get_symbols_to_update(control_file:Path, test_file:Path):
    assert(control_file.exists() and test_file.exists())
    control_lines = control_file.read_text().split("\n")
    test_lines = test_file.read_text().split("\n")

    class LineRef:
        def __init__(self) -> None:
            self.line = 0

    last_test_line = LineRef()

    def get_equivalent_test_func(func_header:str, last_test_line:LineRef):
        # .text:0x0 | 0x39054 | size: 0x35C
        size = func_header.split("size: ")[1]
        test_size = f"| size: {size}"

        start_ind = None
        for i in range(last_test_line.line, len(test_lines)):
            line = test_lines[i] 
            if line.startswith("# .text:") and test_size in line:
                start_ind = i + 1
            elif line.startswith(".endfn") and start_ind != None:
                last_test_line.line = i
                return test_lines[start_ind:i]

    new_links:dict[str, str] = {}
    for control_line_ind, line in enumerate(control_lines):
        if line.startswith(".fn"):
            start_func_l = control_line_ind
        elif line.startswith(".endfn"):
            end_func_l = control_line_ind
            control_func = control_lines[start_func_l:end_func_l]
            test_func = get_equivalent_test_func(control_lines[start_func_l-1], last_test_line)
            if (test_func == None):
                break
            compare_funcs_line_by_line(control_func, test_func, new_links)

    return new_links

from dataclasses import dataclass
from typing import Optional

@dataclass
class SymbolInfo:
    name:str
    section:str
    addr:str
    size:str
    visibility:Optional[str]
    type:Optional[str] = None
    align:Optional[str] = None
    data:Optional[str] = None

    def combine(self, o:SymbolInfo):
        return SymbolInfo(
            self.name, self.section, self.addr, self.size,
            self.visibility if self.visibility  else (o.visibility   if o.visibility else None),
            self.type       if self.type        else (o.type         if o.type       else None),
            self.align      if self.align       else (o.align        if o.align      else None),
            self.data       if self.data        else (o.data         if o.data       else None),
        )
    
    def __str__(self) -> str:
        attributes:list[str] = []
        if self.type:
            attributes.append(f"type:{self.type}")
        attributes.append(f"size:{self.size}")
        if self.visibility:
            attributes.append(f"scope:{self.visibility}")
        if self.align:
            attributes.append(f"align:{self.align}")
        if self.data:
            attributes.append(f"data:{self.data}")
        return f"{self.name} = {self.section}:{self.addr}; // {' '.join(attributes)}"
    
    @staticmethod    
    def from_str(s:str):
        parts = s.split(" ")
        # print(parts)
        name = parts[0]
        section = parts[2].split(":")[0]
        addr = parts[2].split(":")[1].strip(";")
        attributes = s.split("// ")[1].split(" ")
        visibility = None
        alignment = None
        type = None
        data = None
        size = "0x0"
        for attr in attributes:
            type_name, value = attr.split(":")
            match type_name:
                case "type":
                    type = value
                case "align":
                    alignment = value
                case "scope":
                    visibility = value
                case "data":
                    data = value
                case "size":
                    size = value
        return SymbolInfo(name, section, addr, size, visibility, type, alignment, data)

def search_obj_for_symbol_info(obj_file:Path, symbols:list[str]):
    lines = obj_file.read_text()
    out_info:dict[str, SymbolInfo] = {}
    for i, line in enumerate(lines):
        if line.startswith(".fn") or line.startswith(".obj"):
            name = line.split(" ")[1].strip("\",")
            if name in symbols:
                prev_line = lines[i-1]
                addr = prev_line.split(" | ")[1]
                section = prev_line.split(":")[0].strip("# ")
                visibility = line.split(", ")[1]
                size = prev_line.split("size: ")[1]

                out_info[name] = SymbolInfo(name, section, addr, size, visibility)

    return out_info

def search_symbol_file_for_symbol_info(symbol_file:Path, symbols:list[str]):
    lines = symbol_file.read_text().split("\n")

    out: list[SymbolInfo] = []

    for line in lines:
        try:
            info = SymbolInfo.from_str(line)
            if info.name in symbols:
                out.append(info)
        except:
            pass
    return out

def update_symbols(control_file:Path, control_symbols:Path, test_file:Path, test_symbols:Path):
    symbols_to_change = Get_symbols_to_update(control_file, test_file)
    print(symbols_to_change)

    control_local_symbol_info = search_obj_for_symbol_info(control_file, list(symbols_to_change.values()))
    test_local_symbol_info = search_obj_for_symbol_info(test_file, list(symbols_to_change.keys()))

    control_all_symbol_info = search_symbol_file_for_symbol_info(control_symbols, list(symbols_to_change.values()))
    test_other_symbol_info = search_symbol_file_for_symbol_info(test_symbols, list(symbols_to_change.keys()))

    symbol_file_lines = test_symbols.read_text().split("\n")

    for i in range(len(symbol_file_lines)):
        line = symbol_file_lines[i]
        try:
            info = SymbolInfo.from_str(line)
        except:
            continue
        new_name = symbols_to_change.get(info.name)
        if new_name == None:
            continue

        new_name_info = control_local_symbol_info.get(new_name)
        if new_name_info == None:
            potential_names = [x for x in control_all_symbol_info if x.name == new_name]
            assert(len(potential_names) == 1), f"{info.name}, {potential_names}"
            new_name_info = potential_names[0]

        new_info = info.combine(new_name_info)
        new_info.name = new_name_info.name

        symbol_file_lines[i] = str(new_info)

    test_symbols.write_text("\n".join(symbol_file_lines))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("control_file")
    parser.add_argument("control_symbol_file")
    parser.add_argument("test_file")
    parser.add_argument("test_symbol_file")
    args = parser.parse_args()

    update_symbols(Path(args.control_file), Path(args.control_symbol_file), Path(args.test_file), Path(args.test_symbol_file))


if __name__ == "__main__":
    main()
