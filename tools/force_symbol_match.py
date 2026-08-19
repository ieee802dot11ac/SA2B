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

def update_symbols(control_file:Path, test_file:Path, out_symbols:Path):
    new_symbols = Get_symbols_to_update(control_file, test_file)
    print(new_symbols)
    assert(out_symbols.exists())
    sym_text = out_symbols.read_text()
    for old_sym_name, new_sym_name in new_symbols.items():
        sym_text = sym_text.replace(old_sym_name + " ", new_sym_name)
    out_symbols.write_text(sym_text)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("control_file")
    parser.add_argument("test_file")
    parser.add_argument("out_symbol_file")
    args = parser.parse_args()

    update_symbols(Path(args.control_file), Path(args.test_file), Path(args.out_symbol_file))


if __name__ == "__main__":
    main()
