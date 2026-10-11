// ══════════════════════════════════════════════════════
//  CinemaUSAC – script.js (Fase 3 - Sección 3 Completa)
// ══════════════════════════════════════════════════════

const API = 'http://localhost:18080';

// ─── Referencias DOM ──────────────────────────────────
const loginModule   = document.getElementById('login-module');
const adminPanel    = document.getElementById('admin-panel');
const clientPanel   = document.getElementById('client-panel');
const loginForm     = document.getElementById('login-form');
const loginError    = document.getElementById('login-error');
const clientWelcome = document.getElementById('client-welcome');

// Carga Masiva
const dropZone      = document.getElementById('drop-zone');
const fileInput     = document.getElementById('json-file-input');
const fileInfo      = document.getElementById('file-info');
const fileNameDisp  = document.getElementById('file-name-display');
const uploadBtn     = document.getElementById('upload-btn');
const logConsole    = document.getElementById('log-console');
const statsCard     = document.getElementById('stats-card');

// Tabs Admin
const tabBtnCarga   = document.getElementById('tab-btn-carga');
const tabBtnGrafo   = document.getElementById('tab-btn-grafo');
const tabPaneCarga  = document.getElementById('tab-pane-carga');
const tabPaneGrafo  = document.getElementById('tab-pane-grafo');

// BFS Camino Mínimo
const bfsOrigen     = document.getElementById('bfs-origen');
const bfsDestino    = document.getElementById('bfs-destino');
const btnCalcularBFS= document.getElementById('btn-calcular-bfs');
const bfsResBox     = document.getElementById('bfs-resultado-contenedor');
const bfsSaltosVal  = document.getElementById('bfs-saltos-valor');
const bfsRutaFlow   = document.getElementById('bfs-ruta-flow');

// Ranking Aristas
const rankingBody   = document.getElementById('ranking-table-body');

// Reportes Graphviz
const btnVerGrafo   = document.getElementById('btn-ver-grafo-sedes');
const btnVerLista   = document.getElementById('btn-ver-lista-ady');
const reporteImg    = document.getElementById('reporte-imagen');
const reporteHold   = document.getElementById('reporte-placeholder');
const reporteLoad   = document.getElementById('reporte-loading');
const reporteActions= document.getElementById('reporte-actions');
const btnDescargaDot= document.getElementById('btn-descargar-dot');

// Cliente
const clientSedeSel = document.getElementById('client-sede-select');
const clientCardsG  = document.getElementById('client-cards-grid');
const clientHold    = document.getElementById('client-placeholder');

// Modal Imagen
const imageModal    = document.getElementById('image-modal');
const modalImg      = document.getElementById('modal-img');
const modalTitulo   = document.getElementById('modal-titulo');

let selectedFileContent = null;
let catalogoSedesGlobal = [];
let tipoReporteActual   = 'sedes';

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
                cambiarTab('carga');
                await cargarCatalogoSedes();
                await cargarRankingAristas();
            } else {
                clientWelcome.textContent = data.message || `Bienvenido ${data.nombre || 'Cliente'}`;
                show(clientPanel);
                await cargarCatalogoSedes();
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

// ══ TABS ADMIN ════════════════════════════════════════
function cambiarTab(tab) {
    if (tab === 'carga') {
        tabBtnCarga.classList.add('active');
        tabBtnGrafo.classList.remove('active');
        tabPaneCarga.classList.add('active');
        tabPaneGrafo.classList.remove('active');
    } else {
        tabBtnGrafo.classList.add('active');
        tabBtnCarga.classList.remove('active');
        tabPaneGrafo.classList.add('active');
        tabPaneCarga.classList.remove('active');

        // Cargar datos del grafo si aún no se han cargado
        cargarCatalogoSedes();
        cargarRankingAristas();
        if (reporteImg.src === '' || reporteImg.style.display === 'none') {
            mostrarReporte('sedes');
        }
    }
}

// ══ DROP ZONE – CARGA MASIVA ══════════════════════════
dropZone.addEventListener('click', () => fileInput.click());

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

        const ok = data.status === 'success';
        logLine(`${ok ? '✅' : '⚠'} Estado: ${data.status.toUpperCase()}`, ok ? 'success' : 'warn');
        logLine(`   🏢 Sedes cargadas:       ${data.sedes_cargadas}`,     'success');
        logLine(`   🎬 Películas cargadas:   ${data.peliculas_cargadas}`, 'success');
        logLine(`   🎟  Funciones cargadas:  ${data.funciones_cargadas}`, 'success');
        logLine(`   🕸️  Aristas recalculadas: ${data.aristas_conectadas ?? 0}`, 'info');

        if (data.funciones_rechazadas > 0) {
            logLine(`   ❌ Funciones rechazadas: ${data.funciones_rechazadas}`, 'error');
        }

        if (data.errores && data.errores.length > 0) {
            logLine('── Bitácora de Errores ──────────────────', 'dim');
            data.errores.forEach(msg => logLine(`  ${msg}`, 'error'));
        }

        if (data.advertencias && data.advertencias.length > 0) {
            logLine('── Advertencias ─────────────────────────', 'dim');
            data.advertencias.forEach(msg => logLine(`  ${msg}`, 'warn'));
        }

        logLine('──────────────────────────────────────', 'dim');
        actualizarStats(data);

        // Actualizar catálogos y ranking en caliente
        await cargarCatalogoSedes();
        await cargarRankingAristas();

    } catch (err) {
        logLine(`❌ Error de red: ${err.message}`, 'error');
        logLine('   Verifica que el servidor C++ esté corriendo.', 'warn');
    }

    uploadBtn.disabled = false;
}

// ══ CONSOLA Y STATS ═══════════════════════════════════
function logLine(msg, type = 'info') {
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

function actualizarStats(data) {
    document.getElementById('stat-sedes').textContent    = data.sedes_cargadas      ?? 0;
    document.getElementById('stat-pelis').textContent    = data.peliculas_cargadas  ?? 0;
    document.getElementById('stat-funcs').textContent    = data.funciones_cargadas  ?? 0;
    document.getElementById('stat-rejected').textContent = data.funciones_rechazadas ?? 0;
    statsCard.classList.remove('hidden');
    statsCard.classList.add('active');
    statsCard.style.display = 'block';
}

// ══════════════════════════════════════════════════════
// SECCIÓN 3: GRAFO DE SEDES, BFS, RANKING Y GRAPHVIZ
// ══════════════════════════════════════════════════════

// ── 1. Cargar catálogo de Sedes para dropdowns ─────────
async function cargarCatalogoSedes() {
    try {
        const res = await fetch(`${API}/api/sedes`);
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            catalogoSedesGlobal = data.sedes || [];
            poblarSelectSedes(bfsOrigen,  catalogoSedesGlobal, 'Selecciona origen...');
            poblarSelectSedes(bfsDestino, catalogoSedesGlobal, 'Selecciona destino...');
            poblarSelectSedes(clientSedeSel, catalogoSedesGlobal, 'Selecciona tu sede habitual...');
        }
    } catch (err) {
        console.warn('No se pudo cargar el catálogo de sedes:', err);
    }
}

function poblarSelectSedes(selectEl, listaSedes, placeholder) {
    if (!selectEl) return;
    const valorPrevio = selectEl.value;
    selectEl.innerHTML = `<option value="">${placeholder}</option>`;

    listaSedes.forEach(s => {
        const opt = document.createElement('option');
        opt.value = s.codigo;
        opt.textContent = `[${s.codigo}] ${s.nombre}`;
        selectEl.appendChild(opt);
    });

    if (valorPrevio && listaSedes.some(s => s.codigo === valorPrevio)) {
        selectEl.value = valorPrevio;
    }
}

// ── 2. Calcular Camino Mínimo (BFS) ───────────────────
async function calcularCaminoCortoBFS() {
    const origen  = bfsOrigen.value;
    const destino = bfsDestino.value;

    if (!origen || !destino) {
        alert('Por favor selecciona tanto la sede de origen como la de destino.');
        return;
    }

    btnCalcularBFS.disabled = true;
    btnCalcularBFS.innerHTML = '<span>⏳ Calculando ruta con BFS...</span>';

    try {
        const res = await fetch(`${API}/api/sedes/camino-corto`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ origen, destino })
        });
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            bfsSaltosVal.textContent = data.saltos;
            bfsRutaFlow.innerHTML = '';

            const ruta = data.ruta || [];
            ruta.forEach((nodo, index) => {
                const esOrigen  = (index === 0);
                const esDestino = (index === ruta.length - 1);

                const card = document.createElement('div');
                card.className = `route-node-card ${esOrigen ? 'origin' : ''} ${esDestino ? 'dest' : ''}`;
                card.innerHTML = `
                    <span class="route-node-code">${esOrigen ? '🚩 ORIGEN: ' : esDestino ? '🎯 DESTINO: ' : '📍 INTERMEDIO: '}${nodo.codigo}</span>
                    <strong class="route-node-name">${nodo.nombre}</strong>
                    <small style="color:var(--text-muted);font-size:.75rem">Salas: ${nodo.salas} · Tel: ${nodo.telefono}</small>
                `;
                bfsRutaFlow.appendChild(card);

                if (index < ruta.length - 1) {
                    const arrow = document.createElement('span');
                    arrow.className = 'route-arrow';
                    arrow.textContent = '➔';
                    bfsRutaFlow.appendChild(arrow);
                }
            });

            show(bfsResBox);
        } else {
            bfsRutaFlow.innerHTML = `
                <p style="color:var(--error);padding:.5rem;font-weight:600;">
                    ❌ ${data.message || 'No se encontró conexión entre las sedes.'}
                </p>
            `;
            bfsSaltosVal.textContent = '-';
            show(bfsResBox);
        }
    } catch (err) {
        alert('Error al calcular el camino mínimo: ' + err.message);
    } finally {
        btnCalcularBFS.disabled = false;
        btnCalcularBFS.innerHTML = '<span>🔍 Calcular Ruta Más Corta</span>';
    }
}

// ── 3. Cargar y Renderizar Ranking de Aristas ─────────
async function cargarRankingAristas() {
    try {
        const res = await fetch(`${API}/api/sedes/ranking`);
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            const ranking = data.ranking || [];

            if (ranking.length === 0) {
                rankingBody.innerHTML = `
                    <tr>
                        <td colspan="4" style="text-align:center;color:var(--text-dim);padding:1.5rem">
                            No hay conexiones registradas con películas en común.
                        </td>
                    </tr>
                `;
                return;
            }

            rankingBody.innerHTML = '';
            ranking.forEach(item => {
                let medalla = `#${item.posicion}`;
                let posClass = '';
                if (item.posicion === 1) { medalla = '🥇 1°'; posClass = 'rank-pos-1'; }
                else if (item.posicion === 2) { medalla = '🥈 2°'; posClass = 'rank-pos-2'; }
                else if (item.posicion === 3) { medalla = '🥉 3°'; posClass = 'rank-pos-3'; }

                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td class="rank-pos ${posClass}">${medalla}</td>
                    <td><strong>[${item.origen_codigo}]</strong> ${item.origen_nombre}</td>
                    <td><strong>[${item.destino_codigo}]</strong> ${item.destino_nombre}</td>
                    <td>
                        <span class="affinity-badge">🔥 ${item.peliculas_comun} ${item.peliculas_comun === 1 ? 'película' : 'películas'}</span>
                    </td>
                `;
                rankingBody.appendChild(tr);
            });
        }
    } catch (err) {
        console.warn('Error al cargar ranking de aristas:', err);
    }
}

// ── 4. Visualizador de Reportes Graphviz en Tiempo Real ──
async function mostrarReporte(tipo) {
    tipoReporteActual = tipo;

    if (tipo === 'sedes') {
        btnVerGrafo.classList.add('active');
        btnVerLista.classList.remove('active');
    } else {
        btnVerLista.classList.add('active');
        btnVerGrafo.classList.remove('active');
    }

    const endpoint = (tipo === 'sedes')
        ? `${API}/api/grafo/reporte-sedes`
        : `${API}/api/grafo/reporte-adyacencia`;

    hide(reporteHold);
    hide(reporteImg);
    hide(reporteActions);
    show(reporteLoad);

    try {
        const res = await fetch(endpoint);
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            reporteImg.src = data.url;
            reporteImg.onload = () => {
                hide(reporteLoad);
                show(reporteImg);
                show(reporteActions);
                btnDescargaDot.href = data.dot_url;
            };
            reporteImg.onerror = () => {
                hide(reporteLoad);
                reporteHold.textContent = '⚠ La imagen del reporte aún no está lista o hubo un fallo en el renderizado.';
                show(reporteHold);
            };
        } else {
            hide(reporteLoad);
            reporteHold.textContent = `❌ ${data.mensaje || 'Error al generar el reporte.'}`;
            show(reporteHold);
        }
    } catch (err) {
        hide(reporteLoad);
        reporteHold.textContent = '❌ Error de comunicación con el servidor: ' + err.message;
        show(reporteHold);
    }
}

function regenerarReporteActual() {
    mostrarReporte(tipoReporteActual);
}

// ── 5. Modal de Zoom de Imagen ────────────────────────
function abrirModalImagen() {
    if (!reporteImg.src) return;
    modalImg.src = reporteImg.src;
    modalTitulo.textContent = (tipoReporteActual === 'sedes')
        ? 'Reporte Completo: Grafo de Sedes'
        : 'Reporte Completo: Lista de Adyacencia';
    show(imageModal);
}

function cerrarModalImagen() {
    hide(imageModal);
}

// ── 6. Portal de Clientes: Sedes Similares / Sugeridas ─
async function cargarSedesSimilaresCliente() {
    const codigoSede = clientSedeSel.value;
    if (!codigoSede) {
        clientCardsG.innerHTML = '';
        hide(clientCardsG);
        show(clientHold);
        return;
    }

    hide(clientHold);
    clientCardsG.innerHTML = '<p style="color:var(--text-muted);padding:1rem">Cargando sedes con catálogo similar...</p>';
    show(clientCardsG);

    try {
        const res = await fetch(`${API}/api/sedes/similares?codigo=${encodeURIComponent(codigoSede)}`);
        const data = await res.json();

        if (res.ok && data.status === 'success') {
            const similares = data.similares || [];

            if (similares.length === 0) {
                clientCardsG.innerHTML = `
                    <div style="grid-column: 1/-1; padding: 2rem; text-align: center; color: var(--text-muted);">
                        🏢 Esta sede no tiene películas en común con otras sedes en este momento.
                    </div>
                `;
                return;
            }

            clientCardsG.innerHTML = '';
            similares.forEach(s => {
                const card = document.createElement('div');
                card.className = 'recommendation-card';
                card.innerHTML = `
                    <div class="rec-header">
                        <span class="rec-code">${s.codigo}</span>
                        <span style="font-size:1.3rem">🍿</span>
                    </div>
                    <h4 class="rec-title">${s.nombre}</h4>
                    <div class="rec-meta">
                        <span>📍 ${s.direccion}</span>
                        <span>🎬 Salas Disponibles: ${s.salas}</span>
                        <span>📞 Tel: ${s.telefono}</span>
                    </div>
                    <div class="rec-affinity">
                        <span>🔥</span>
                        <span>${s.peliculas_comun} ${s.peliculas_comun === 1 ? 'película compartida' : 'películas compartidas'} en cartelera</span>
                    </div>
                `;
                clientCardsG.appendChild(card);
            });
        } else {
            clientCardsG.innerHTML = `<p style="color:var(--error);padding:1rem">❌ ${data.message || 'Error al obtener recomendaciones.'}</p>`;
        }
    } catch (err) {
        clientCardsG.innerHTML = `<p style="color:var(--error);padding:1rem">❌ Error de conexión: ${err.message}</p>`;
    }
}

// ══ HELPERS ════════════════════════════════════════════
function show(el) { if (el) { el.classList.remove('hidden'); el.classList.add('active'); el.style.display = ''; } }
function hide(el) { if (el) { el.classList.remove('active'); el.classList.add('hidden'); el.style.display = 'none'; } }
