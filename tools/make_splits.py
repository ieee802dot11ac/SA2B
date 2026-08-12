from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from sys import argv
from typing import Optional

def extract(in_path:Path, out_path:Path):
    in_symbols = in_path/"symbols.txt"

    rel_name = in_path.parts[-1]

    build_path = Path("build") / "GSNE8P" / rel_name / "asm"
    print(rel_name)
    print(build_path)

    text_ranges: dict[str, range] = {}
    text_rodata_usage: dict[str, set[str]] = {}

    rodata_ranges: dict[str, range] = {}
    rodata_starts: dict[int, str] = {}

    for line in in_symbols.read_text().split("\n"):
        if "type:function" in line:
            parts = line.split(" ")
            name = parts[0]
            size = None
            addr = None
            for part in parts:
                if part.startswith("size:"):
                    size = int(part.split(":")[1], 16)
                elif part.startswith(".text:"):
                    addr = int(part.split(":")[1].strip(";"), 16)
            assert(size!=None and addr!=None)
            text_ranges[name] = range(addr, addr+size)
        elif ".rodata" in line:
            parts = line.split(" ")
            name = parts[0]
            size = None
            addr = None
            for part in parts:
                if part.startswith("size:"):
                    size = int(part.split(":")[1], 16)
                elif part.startswith(".rodata:"):
                    addr = int(part.split(":")[1].strip(";"), 16)
            assert(size!=None and addr!=None)
            start = addr & ~7 # because rodata has to extend to a period of 8 bytes, if something doesn't start at 8 bytes, we can roll back
            end = (addr + size + 7) & ~7 # round up to a multiple of 8
            rodata_ranges[name] = range(start, end)
            rodata_starts[addr] = name

    for file in [x for x in build_path.iterdir() if x.is_file()]:
        current_function = None
        current_func_set = None
        for line in file.read_text().split("\n"):
            if line.startswith(".fn "):
                current_function = line.split(" ")[1].strip(",")
                current_func_set = set()
                continue

            if "_rodata_" in line and (current_func_set != None):
                rodata_name = line.split(" ")[-1].split("@")[0]
                # print(rodata_name)
                current_func_set.add(rodata_name)

            if line.startswith(".endfn") and current_func_set:
                text_rodata_usage[current_function] = current_func_set
                current_function = None
                current_func_set = None



    # print(text_rodata_usage)
    rodata_text_usage: dict[str, set[str]] = {}
    for fn, rodatas in text_rodata_usage.items():
        for rodata_name in rodatas:
            d = rodata_text_usage.setdefault(rodata_name, set())
            d.add(fn)
            
    # print(rodata_text_usage)

    class MyRange:
        def __init__(self, start:int, stop:int) -> None:
            self.start = start
            self.stop = stop
        def ToRange(self):
            return range(self.start, self.stop)
        
        @staticmethod
        def FromRange(r:range):
            return MyRange(r.start, r.stop)

        def __str__(self) -> str:
            return f"{self.start:#x} -> {self.stop:#x}"

        def __repr__(self) -> str:
            return self.__str__()

        def __hash__(self) -> int:
            return hash((self.start, self.stop))

        def __eq__(self, value: object) -> bool:
            return isinstance(value, MyRange) and hash(self) == hash(value)

    def combine_ranges(r1: Optional[MyRange], r2: Optional[MyRange]) -> Optional[MyRange]:
        if r1:
            if r2:
                return MyRange(min(r1.start, r2.start), max(r1.stop, r2.stop))
            else:
                return r1
        elif r2:
            return r2
        else:
            return None

    def overlaps(r1: Optional[MyRange], r2: Optional[MyRange]):
        if not r1 or not r2:
            return False
        
        minRange, maxRange = (r1, r2) if r1.start <= r2.start else (r2, r1)

        return (
            # same start
            (minRange.start == maxRange.start) or
            # same end
            (minRange.stop == maxRange.stop) or
            # max inside min
            (maxRange.stop < minRange.stop) or
            # min extends into max
            (minRange.stop > maxRange.start)        
        )

    @dataclass
    class RodataTextRange:
        rodataRange: Optional[MyRange] = None
        textRange: Optional[MyRange] = None
        def __hash__(self) -> int:
            return hash((self.rodataRange, self.textRange))
        def __str__(self) -> str:
            return f"text: {self.textRange}, rodata: {self.rodataRange}"
        def __repr__(self) -> str:
            return self.__str__()
        def __eq__(self, value: object) -> bool:
            return isinstance(value, RodataTextRangeWrapper) and hash(self) == hash(value)

    @dataclass
    class RodataTextRangeWrapper:
        wrap: RodataTextRange
        def __eq__(self, value: object) -> bool:
            return isinstance(value, RodataTextRangeWrapper) and self.wrap == value.wrap
        def __hash__(self) -> int:
            return hash(self.wrap)

    symbol_to_file: dict[str, RodataTextRangeWrapper] = {}

    def AddSymbol(currFile: RodataTextRangeWrapper, rodata_name: str):
        rodata_file = symbol_to_file.get(rodata_name)
        if (rodata_file == None):
            currFile.wrap.rodataRange = combine_ranges(currFile.wrap.rodataRange, MyRange.FromRange(rodata_ranges[rodata_name]))
            symbol_to_file[rodata_name] = currFile
        else:
            currFile.wrap.rodataRange = combine_ranges(rodata_file.wrap.rodataRange, currFile.wrap.rodataRange)
            currFile.wrap.textRange = combine_ranges(rodata_file.wrap.textRange, currFile.wrap.textRange)
            rodata_file.wrap = currFile.wrap
        


    for fn_name, rodatas in text_rodata_usage.items():
        cumulative_file = RodataTextRangeWrapper(RodataTextRange(textRange=MyRange.FromRange(text_ranges[fn_name])))

        for rodata_name in rodatas:
            for s in rodata_ranges[rodata_name]:
                new_rodata_name = rodata_starts.get(s)
                if (new_rodata_name != None):
                    AddSymbol(cumulative_file, new_rodata_name)

    all_splits = list(symbol_to_file.values())
    for i, base_split in enumerate(all_splits):
        for j, sub_split in enumerate(all_splits[i:]):
            if (overlaps(base_split.wrap.rodataRange, sub_split.wrap.rodataRange) or
                overlaps(base_split.wrap.textRange, sub_split.wrap.textRange)):
                base_split.wrap.rodataRange = combine_ranges(base_split.wrap.rodataRange, sub_split.wrap.rodataRange)
                base_split.wrap.textRange = combine_ranges(base_split.wrap.textRange, sub_split.wrap.textRange)
                sub_split.wrap = base_split.wrap

    all_files = sorted(list(set(x.wrap for x in all_splits)), key=lambda x: x.textRange.start)

    out_lines = []
    for i, r in enumerate(all_files):
        out_lines.append(f"{rel_name}_{i}.c:\n\t.text\tstart:{r.textRange.start:#010X} end:{r.textRange.stop:#010X}\n\t.rodata\tstart:{r.rodataRange.start:#010X} end:{r.rodataRange.stop:#010X}")
    header  = """
Sections:
	.text       type:code align:4
	.ctors      type:rodata align:4
	.dtors      type:rodata align:4
	.rodata     type:rodata align:8
	.data       type:data align:8
	.bss        type:bss align:8
"""
    out_path.write_text(header + "\n".join(out_lines))


if __name__ == "__main__":
    files = [
        "ChaoMain",
        "ChaoStgDark",
        "ChaoStgEntrance",
        "ChaoStgHero",
        "ChaoStgKarate",
        "ChaoStgKinder",
        "ChaoStgLobby",
        "ChaoStgLobby000",
        "ChaoStgLobby00K",
        "ChaoStgLobby0DK",
        "ChaoStgLobbyH0K",
        "ChaoStgLobbyHDK",
        "ChaoStgNeut",
        "ChaoStgOdekake",
        "ChaoStgRace",
        "ChaoStgRaceDark",
        "ChaoStgRaceHero",
        "ChaoStgRaceNeut",
        "ChaoStgStadium",
    ]
    for folder in map(lambda x: Path("./config") / "GSNE8P" / x, files):
        print(folder)
        rel_name = folder.parts[-1]
        if rel_name in ["stg54D", "stg56D"]:
            continue

        in_path = folder
        out_file = folder / "splits.txt"
        try:
            extract(in_path, out_file)
        except Exception as e:
            print(e)
