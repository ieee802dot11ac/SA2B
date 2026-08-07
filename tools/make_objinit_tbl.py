from sys import argv
from itertools import batched
from struct import unpack

file = argv[1]
obj_name = argv[2]

with open(file, "r") as f:
    lines = f.read().split("\n")

begin = f".obj {obj_name},"
end = f".endobj {obj_name}"

strings:dict[str, str] = {}

for i, line in enumerate(lines):
    if line.startswith(begin):
        begin_line = i
    elif line.startswith(end):
        end_line = i
    elif ".string \"" in line:
        obj_name = lines[i-1].split(" ")[1].strip(",")
        string_val = line[10:-1]
        strings[obj_name] = string_val
        # print(string_val)

struct_lines = lines[begin_line+1:end_line]

IM_FLAGS = {1:"IM_MOTIONWK", 2:"IM_TASKWK", 4:"IM_FORCEWK", 8:"IM_ANYWK"}
ITEM_ATTR = {
    1:"ITEM_ATTR_DIST",
    2:"ITEM_ATTR_LOAD",
    4:"ITEM_ATTR_ONCE",
    0x10:"ITEM_ATTR_RING",
    0x20:"ITEM_ATTR_RING_GROUP",
    0x30:"ITEM_ATTR_RING_ITEMBOX",
    0x80:"ITEM_ATTR_EMERALD",
}
TASKLVL = {
    0:"LEV_0",
    1:"LEV_1",
    2:"LEV_2",
    3:"LEV_3",
    4:"LEV_4",
    5:"LEV_5",
    6:"LEV_6",
    7:"LEV_C",
    8:"LEV_M",
}

out_lines = []
for objDef in batched(struct_lines, 4):
    # print(objDef)
    flags, fRange, fnExec, strObjName = map(lambda x: x.split(".4byte ")[1], objDef)
    flags = int(flags, 16)
    fRange = unpack(">f", int(fRange, 16).to_bytes(4, "big"))[0]
    im_flag = (flags >> 24) & 0xff
    task_lvl = (flags >> 16) & 0xff
    item_attr = flags & 0xffff
    strObjName = strings[strObjName]

    im_flags = " | ".join([name for flag, name in IM_FLAGS.items() if im_flag & flag])

    im_flags = [name for flag, name in IM_FLAGS.items() if im_flag & flag]
    if len(im_flags) > 0:
        im_flags = " | ".join(im_flags)
    else:
        im_flags = 0

    item_flags = [name for flag, name in ITEM_ATTR.items() if item_attr & flag]
    if len(item_flags) > 0:
        item_flags = " | ".join(item_flags)
    else:
        item_flags = 0

    task_flag = TASKLVL.get(task_lvl, task_lvl)

    out_lines.append(f"{{ {im_flags}, {task_flag}, {item_flags}, {fRange}f, &{fnExec}, \"{strObjName}\" }},")
print("\n".join(out_lines))
print(len(out_lines))
