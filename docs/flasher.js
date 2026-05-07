(function () {
  'use strict';

  const VERSION_URL = './version.json';

  const statusEl       = document.getElementById('status');
  const versionEl      = document.getElementById('version-info');
  const browserWarning = document.getElementById('browser-warning');
  const instructions   = document.getElementById('instructions');

  // ── Browser support check ────────────────────────────────────────────────
  if (!('showDirectoryPicker' in window)) {
    browserWarning.hidden = false;
    instructions.hidden   = true;
    document.querySelectorAll('.flash-btn').forEach(b => b.disabled = true);
    return;
  }

  // ── Version stamp ────────────────────────────────────────────────────────
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

  // ── Flash a single UF2 URL ───────────────────────────────────────────────
  async function flash(firmwareUrl) {
    const allBtns = document.querySelectorAll('.flash-btn');
    allBtns.forEach(b => b.disabled = true);

    try {
      setStatus('Fetching firmware…', 'info');
      const res = await fetch(firmwareUrl);
      if (!res.ok) throw new Error(`Firmware not found (HTTP ${res.status}). Try reloading.`);
      const firmware = await res.arrayBuffer();

      setStatus('Select the GRANDCNT drive in the dialog that opens…', 'info');
      let dir;
      try {
        dir = await window.showDirectoryPicker({ mode: 'readwrite' });
      } catch (e) {
        if (e.name === 'AbortError') { setStatus('Cancelled.', ''); return; }
        throw e;
      }

      setStatus('Writing — do not unplug…', 'info');
      const file     = await dir.getFileHandle('firmware.uf2', { create: true });
      const writable = await file.createWritable();
      await writable.write(firmware);
      await writable.close();

      setStatus('Done — board is rebooting into new firmware.', 'success');

    } catch (e) {
      setStatus('Error: ' + e.message, 'error');
    } finally {
      allBtns.forEach(b => b.disabled = false);
    }
  }

  // ── Wire up buttons ──────────────────────────────────────────────────────
  document.getElementById('flash-btn-pcb1')
    .addEventListener('click', () => flash('./firmware_pcb1.uf2'));

  document.getElementById('flash-btn-pcb2')
    .addEventListener('click', () => flash('./firmware_pcb2.uf2'));

})();
