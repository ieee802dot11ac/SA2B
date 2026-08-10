import argparse
from dataclasses import dataclass

class prs_reader:
    @dataclass
    class literal:
        v: int
        def run(self, b:bytearray) -> bool:
            b.append(self.v)
            return True

    @dataclass
    class lookback:
        offset: int
        count: int
        def run(self, b:bytearray) -> bool:
            if (self.offset==0):
                return False
            if len(b) < self.offset:
                raise RuntimeError("read out of bounds")
            offs = -self.offset
            for _ in range(self.count):
                b.append(b[offs])
            return True
    
    def __init__(self, b: bytes) -> None:
        self.buff = b
        self.buff_ind = 0
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
        ret = self.buff[self.buff_ind]
        self.buff_ind += 1
        return ret

    def read_s16(self) -> int:
        ret = int.from_bytes(self.buff[self.buff_ind : self.buff_ind+2], "little", signed=True)
        self.buff_ind += 2
        return ret

    def read_command(self):
        if self.read_bit():
            return self.literal(self.read_u8())

        if self.read_bit():
            offset = self.read_s16()
            if offset == 0:
                return None
            size = offset & 0x7
            offset >>= 3
            if size == 0:
                size = self.read_u8() + 1
            else:
                size += 2
            offset |= -0x2000
            return self.lookback(-offset, size)
        else:
            bit_1 = self.read_bit()
            bit_0 = self.read_bit()
            size = ((bit_1 << 1) | bit_0) + 2
            offset = self.read_u8() | -0x100
            return self.lookback(-offset, size)

    
def decompress_prs(b: bytes) -> bytes:
    r = prs_reader(b)
    out_bytes = bytearray()
    run = True
    while run:
        cmd = r.read_command()
        if not cmd:
            break
        run = cmd.run(out_bytes)
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
