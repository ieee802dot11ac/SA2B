from prs_decode import decompress_prs
from pathlib import Path

files = [
    "ChaoMain.prs",
    "ChaoStgDark.prs",
    "ChaoStgEntrance.prs",
    "ChaoStgHero.prs",
    "ChaoStgKarate.prs",
    "ChaoStgKinder.prs",
    "ChaoStgLobby.prs",
    "ChaoStgLobby000.prs",
    "ChaoStgLobby00K.prs",
    "ChaoStgLobby0DK.prs",
    "ChaoStgLobbyH0K.prs",
    "ChaoStgLobbyHDK.prs",
    "ChaoStgNeut.prs",
    "ChaoStgOdekake.prs",
    "ChaoStgRace.prs",
    "ChaoStgRaceDark.prs",
    "ChaoStgRaceHero.prs",
    "ChaoStgRaceNeut.prs",
    "ChaoStgStadium.prs",
]

base_dir = Path("orig") / "GSNE8P" / "files"

for f in files:
    file_p = base_dir / f
    out_p = file_p.with_suffix(".rel")
    assert(file_p.exists()), f"{file_p} doesn't exist"

    out_p.write_bytes(decompress_prs(file_p.read_bytes()))
