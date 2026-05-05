# Web Flasher Plan

## Goal
A GitHub Pages web page where users can flash the latest Synthseqr firmware to their
Adafruit Grand Central M4 Express without installing any software.

## Decisions made
- **Approach**: UF2 + File System Access API (Option A — simpler, no SAM-BA protocol needed)
  - User double-clicks reset on board → mounts as GRANDCNT USB drive
  - Page fetches pre-built firmware.uf2 from GitHub Pages
  - File System Access API writes it to the mounted drive
  - Board auto-reboots into new firmware
- **Browser support**: Chrome/Edge only (Web Serial + File System Access require it; HTTPS required)
- **Repo layout**: Web flasher lives in this same repo under `docs/`
- **Deploy trigger**: Push to master (or optionally tag-based for intentional releases only)

## Repo layout (planned)
```
synthseqr_with_grandcentralM4express/   <- Arduino sketch (unchanged)
docs/
  index.html                            <- web flasher page
  flasher.js
  firmware.uf2                          <- built by CI, never hand-edited
.github/
  workflows/
    build-and-deploy.yml                <- builds firmware, converts to UF2, deploys docs/
```

## GitHub Actions workflow (planned steps)
1. Install arduino-cli
2. Install required libraries (MIDIUSB, FifteenStep)
3. Compile sketch → .bin
4. Convert .bin → firmware.uf2 via uf2conv.py
5. Copy firmware.uf2 into docs/
6. Deploy docs/ to GitHub Pages

## Open questions / next steps
- [ ] Resolve FifteenStep library path in CI
      (`../../../libraries/FifteenStep/` won't exist in GitHub Actions checkout)
      Options: include FifteenStep as a subfolder in this repo, or add a fetch step in the Action
- [ ] Write the GitHub Actions workflow file
- [ ] Write index.html + flasher.js (UF2 fetch + File System Access API write)
- [ ] Enable GitHub Pages on this repo (Settings → Pages → source: docs/ on master)
- [ ] Decide: deploy on every push to master, or only on tags?
- [ ] Test full flow end-to-end on real hardware
