import argparse
from dataclasses import dataclass

class prs_reader:
    @dataclass
    class literal:
        v: int

    @dataclass
    class lookback:
        offset: int
        count: int
    
    def __init__(self, b: bytes) -> None:
        self.buff = memoryview(b)
        self.cmds = 0
        self.rem = 0

    def read_bit(self) -> int:
        if (self.rem == 0):
            self.cmds = self.read_u8()
            self.rem = 8
        ret = self.cmds & 1
        self.cmds >>= 1
        self.rem -= 1
        return ret
    
    def read_u8(self) -> int:
        ret = self.buff[0]
        self.buff = self.buff[1:]
        return ret

    def read_u16(self) -> int:
        ret = int.from_bytes(self.buff[0:2], "little", signed=True)
        self.buff = self.buff[2:]
        return ret

    def read_command(self):
        if self.read_bit():
            return self.literal(self.read_u8())
        elif self.read_bit():
            offset = self.read_u16()
            if offset == 0:
                return
            size = offset & 0x7
            offset >>= 3
            if size == 0:
                size = self.read_u8() + 1
            else:
                size += 2
            offset |= -0x2000
            return self.lookback(-offset, size)
        else:
            flag = self.read_bit()
            bit = self.read_bit()
            size = ((flag << 1) | bit) + 2
            offset = self.read_u8() | -0x100
            return self.lookback(-offset, size)

    
def decompress_prs(b: bytes) -> bytes:
    r = prs_reader(b)
    out_bytes = bytearray()
    while True:
        cmd = r.read_command()
        # print(cmd)
        if isinstance(cmd, prs_reader.literal):
            out_bytes.append(cmd.v)
        elif isinstance(cmd, prs_reader.lookback):
            if (cmd.offset == 0):
                break
            if len(out_bytes) < cmd.offset:
                raise RuntimeError("read out of bounds")
            offs = -cmd.offset
            for _ in range(cmd.count):
                out_bytes.append(out_bytes[offs])
        else:
            break
    return bytes(out_bytes)

def main():
    parse = argparse.ArgumentParser()
    parse.add_argument("input_file", help="InputFile")
    parse.add_argument("dest_file", help="DestFile")
    args = parse.parse_args()
    inp = args.input_file
    outp = args.dest_file
    with open(inp, "rb") as f:
        in_bytes = f.read()
    out_bytes = decompress_prs(in_bytes)
    with open(outp, "wb") as f:
        f.write(out_bytes)

if __name__ == "__main__": main()
