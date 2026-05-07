(function () {
  'use strict';

  const FIRMWARE_URL = './firmware.uf2';
  const VERSION_URL  = './version.json';

  const btn            = document.getElementById('flash-btn');
  const statusEl       = document.getElementById('status');
  const versionEl      = document.getElementById('version-info');
  const browserWarning = document.getElementById('browser-warning');
  const instructions   = document.getElementById('instructions');

  // ── Browser support check ────────────────────────────────────────────────
  if (!('showDirectoryPicker' in window)) {
    browserWarning.hidden = false;
    instructions.hidden   = true;
    btn.disabled          = true;
    return;
  }

  // ── Version stamp (built by CI, optional — page works without it) ────────
  fetch(VERSION_URL)
    .then(r => r.ok ? r.json() : null)
    .then(v => {
      if (!v) return;
      const short = (v.commit || '').slice(0, 7);
      const date  = (v.date  || '').slice(0, 10);
      versionEl.textContent = short && date ? `build ${short} · ${date}` : '';
    })
    .catch(() => {});

  // ── Status helper ────────────────────────────────────────────────────────
  function setStatus(msg, type) {
    statusEl.textContent = msg;
    statusEl.className   = 'status ' + (type || '');
  }

  // ── Main flash flow ──────────────────────────────────────────────────────
  btn.addEventListener('click', async () => {
    btn.disabled = true;

    try {
      // 1. Fetch the UF2 from GitHub Pages
      setStatus('Fetching firmware…', 'info');
      const res = await fetch(FIRMWARE_URL);
      if (!res.ok) throw new Error(`Firmware not found (HTTP ${res.status}). Try reloading.`);
      const firmware = await res.arrayBuffer();

      // 2. Ask the user to pick the GRANDCNT drive
      //    mode:'readwrite' requests write permission in the initial dialog
      //    so the browser doesn't ask a second time during the write.
      setStatus('Select the GRANDCNT drive in the dialog that opens…', 'info');
      let dir;
      try {
        dir = await window.showDirectoryPicker({ mode: 'readwrite' });
      } catch (e) {
        if (e.name === 'AbortError') { setStatus('Cancelled.', ''); return; }
        throw e;
      }

      // 3. Write firmware.uf2 to the drive root
      //    The Adafruit UF2 bootloader watches for any .uf2 write and
      //    flashes it automatically, then reboots the board.
      setStatus('Writing — do not unplug…', 'info');
      const file     = await dir.getFileHandle('firmware.uf2', { create: true });
      const writable = await file.createWritable();
      await writable.write(firmware);
      await writable.close();

      setStatus('Done — board is rebooting into new firmware.', 'success');

    } catch (e) {
      setStatus('Error: ' + e.message, 'error');
    } finally {
      btn.disabled = false;
    }
  });

})();
