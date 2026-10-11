// ══════════════════════════════════════════════════════
//  CinemaUSAC – script.js   (Sección 1 + 2)
// ══════════════════════════════════════════════════════

const API = 'http://localhost:18080';

// ─── Referencias DOM ──────────────────────────────────
const loginModule   = document.getElementById('login-module');
const adminPanel    = document.getElementById('admin-panel');
const clientPanel   = document.getElementById('client-panel');
const loginForm     = document.getElementById('login-form');
const loginError    = document.getElementById('login-error');
const clientWelcome = document.getElementById('client-welcome');

const dropZone      = document.getElementById('drop-zone');
const fileInput     = document.getElementById('json-file-input');
const fileInfo      = document.getElementById('file-info');
const fileNameDisp  = document.getElementById('file-name-display');
const uploadBtn     = document.getElementById('upload-btn');
const logConsole    = document.getElementById('log-console');
const statsCard     = document.getElementById('stats-card');

let selectedFileContent = null;   // contenido JSON en memoria

// ══ LOGIN ═════════════════════════════════════════════
loginForm.addEventListener('submit', async (e) => {
    e.preventDefault();
    loginError.textContent = '';

    const email    = document.getElementById('email').value.trim();
    const password = document.getElementById('password').value;

    try {
        const res  = await fetch(`${API}/api/login`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ email, password })
        });
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            hide(loginModule);
            loginForm.reset();

            if (data.role === 'admin') {
                show(adminPanel);
            } else {
                clientWelcome.textContent = data.message;
                show(clientPanel);
            }
        } else {
            loginError.textContent = data.message || 'Credenciales incorrectas.';
        }
    } catch {
        loginError.textContent = '⚠ No se pudo contactar al servidor C++.';
    }
});

// ══ LOGOUT ════════════════════════════════════════════
function logout() {
    hide(adminPanel);
    hide(clientPanel);
    show(loginModule);
    limpiarArchivo();
}

// ══ DROP ZONE – carga masiva ══════════════════════════

// Clic en el drop-zone abre el selector de archivo
dropZone.addEventListener('click', () => fileInput.click());

// Drag & Drop
dropZone.addEventListener('dragover', (e) => {
    e.preventDefault();
    dropZone.classList.add('drag-over');
});
dropZone.addEventListener('dragleave', () => dropZone.classList.remove('drag-over'));
dropZone.addEventListener('drop', (e) => {
    e.preventDefault();
    dropZone.classList.remove('drag-over');
    const file = e.dataTransfer.files[0];
    if (file) cargarArchivo(file);
});

// Selección mediante input
fileInput.addEventListener('change', () => {
    if (fileInput.files[0]) cargarArchivo(fileInput.files[0]);
});

function cargarArchivo(file) {
    if (!file.name.endsWith('.json')) {
        logLine('⚠ Solo se aceptan archivos .json', 'warn');
        return;
    }
    const reader = new FileReader();
    reader.onload = (ev) => {
        selectedFileContent = ev.target.result;
        fileNameDisp.textContent = file.name;
        show(fileInfo);
        uploadBtn.disabled = false;
        logLine(`📄 Archivo cargado: ${file.name} (${(file.size / 1024).toFixed(1)} KB)`, 'info');
    };
    reader.readAsText(file);
}

function limpiarArchivo() {
    selectedFileContent = null;
    fileInput.value     = '';
    hide(fileInfo);
    uploadBtn.disabled = true;
}

// ══ EJECUTAR CARGA MASIVA ══════════════════════════════
async function ejecutarCargaMasiva() {
    if (!selectedFileContent) return;

    uploadBtn.disabled = true;
    logLine('──────────────────────────────────────', 'dim');
    logLine('⏳ Enviando datos al servidor C++...', 'info');

    try {
        const res  = await fetch(`${API}/api/carga-masiva`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: selectedFileContent
        });
        const data = await res.json();

        // ── Mostrar resumen ──────────────────────────
        const ok = data.status === 'success';
        logLine(`${ok ? '✅' : '⚠'} Estado: ${data.status.toUpperCase()}`, ok ? 'success' : 'warn');
        logLine(`   🏢 Sedes cargadas:       ${data.sedes_cargadas}`,     'success');
        logLine(`   🎬 Películas cargadas:   ${data.peliculas_cargadas}`, 'success');
        logLine(`   🎟  Funciones cargadas:  ${data.funciones_cargadas}`, 'success');

        if (data.funciones_rechazadas > 0) {
            logLine(`   ❌ Funciones rechazadas: ${data.funciones_rechazadas}`, 'error');
        }

        // ── Errores detallados ───────────────────────
        if (data.errores && data.errores.length > 0) {
            logLine('── Bitácora de Errores ──────────────────', 'dim');
            data.errores.forEach(msg => logLine(`  ${msg}`, 'error'));
        }

        // ── Advertencias ─────────────────────────────
        if (data.advertencias && data.advertencias.length > 0) {
            logLine('── Advertencias ─────────────────────────', 'dim');
            data.advertencias.forEach(msg => logLine(`  ${msg}`, 'warn'));
        }

        logLine('──────────────────────────────────────', 'dim');

        // ── Actualizar tarjeta de estadísticas ───────
        actualizarStats(data);

    } catch (err) {
        logLine(`❌ Error de red: ${err.message}`, 'error');
        logLine('   Verifica que el servidor C++ esté corriendo.', 'warn');
    }

    uploadBtn.disabled = false;
}

// ══ CONSOLA DE LOGS ════════════════════════════════════
function logLine(msg, type = 'info') {
    // Eliminar placeholder si existe
    const placeholder = logConsole.querySelector('.log-placeholder');
    if (placeholder) placeholder.remove();

    const line = document.createElement('p');
    line.className = `log-line log-${type}`;
    line.textContent = msg;
    logConsole.appendChild(line);
    logConsole.scrollTop = logConsole.scrollHeight;
}

function limpiarConsola() {
    logConsole.innerHTML = '<p class="log-placeholder">Los resultados de la carga masiva aparecerán aquí...</p>';
}

// ══ ESTADÍSTICAS ═══════════════════════════════════════
function actualizarStats(data) {
    document.getElementById('stat-sedes').textContent    = data.sedes_cargadas      ?? 0;
    document.getElementById('stat-pelis').textContent    = data.peliculas_cargadas  ?? 0;
    document.getElementById('stat-funcs').textContent    = data.funciones_cargadas  ?? 0;
    document.getElementById('stat-rejected').textContent = data.funciones_rechazadas ?? 0;
    statsCard.classList.remove('hidden');
    statsCard.classList.add('active');
    statsCard.style.display = 'block';
}

// ══ HELPERS ════════════════════════════════════════════
function show(el) { el.classList.remove('hidden'); el.classList.add('active'); }
function hide(el) { el.classList.remove('active'); el.classList.add('hidden'); }
