const $ = selector => document.querySelector(selector);
const modal = $('#modal');
let enrollTimer = null;

async function api(url, options = {}) {
  const response = await fetch(url, {
    headers: { 'Content-Type': 'application/json', ...(options.headers || {}) },
    ...options
  });
  const data = await response.json();
  if (!response.ok) throw new Error(data.message || 'Falha na solicitação.');
  return data;
}

function escapeHtml(value) {
  return String(value).replace(/[&<>"']/g, char => ({
    '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;'
  })[char]);
}

async function refresh() {
  try {
    const [status, people, lastRead] = await Promise.all([
      api('/api/status'), api('/api/people'), api('/api/rfid/last')
    ]);
    $('#connection').textContent = `ESP32 conectada • ${status.ip}`;
    $('.connection').classList.add('online');
    $('#address').textContent = `${status.ip} • bateponto.local`;
    const readerConnected = status.rfid === 'connected';
    $('#readerStatusText').textContent = readerConnected ? 'Leitor conectado' : 'Leitor desconectado';
    $('#readerStatus').classList.toggle('offline', !readerConnected);
    $('#count').textContent = people.people.length;
    $('#people').innerHTML = people.people.length
      ? people.people.map(person => `<div class="person"><div class="person-info"><strong>${escapeHtml(person.name)}</strong><code>${escapeHtml(person.uid)}</code></div><button class="remove-person" type="button" data-id="${Number(person.id)}" data-name="${escapeHtml(person.name)}" aria-label="Remover ${escapeHtml(person.name)}">Remover</button></div>`).join('')
      : '<p class="empty">Nenhuma pessoa cadastrada ainda.</p>';

    if (lastRead.uid || lastRead.status === 'timeout' || lastRead.status === 'cancelled') {
      $('#lastName').textContent = lastRead.name || '—';
      $('#lastUid').textContent = lastRead.uid || '—';
      const labels = {
        recognized: 'Cartão reconhecido', unknown: 'Cartão não cadastrado',
        enrolled: 'Cadastro concluído', duplicate: lastRead.name,
        timeout: 'Tempo de cadastro expirado', cancelled: 'Cadastro cancelado'
      };
      $('#lastResult').textContent = labels[lastRead.status] || 'Leitura realizada';
    }
  } catch {
    $('#connection').textContent = 'Sem conexão com a ESP32';
    $('.connection').classList.remove('online');
  }
}

function resetEnrollmentForm() {
  $('#name').disabled = false;
  $('#start').disabled = false;
  $('#start').textContent = 'Iniciar cadastro';
  $('#enrollForm').reset();
}

function stopEnrollmentPolling() {
  if (enrollTimer !== null) clearInterval(enrollTimer);
  enrollTimer = null;
}

$('#addPerson').onclick = () => { modal.showModal(); $('#name').focus(); };
$('#closeModal').onclick = cancelEnrollment;
$('#cancel').onclick = cancelEnrollment;
$('#resetPeople').onclick = async event => {
  const button = event.currentTarget;
  if (!confirm('Apagar todos os cadastros salvos? Esta ação não pode ser desfeita.')) return;
  button.disabled = true;
  try {
    await api('/api/people/reset', { method: 'POST' });
    await refresh();
  } catch (error) {
    alert(error.message);
  } finally {
    button.disabled = false;
  }
};
$('#people').onclick = async event => {
  const button = event.target.closest('.remove-person');
  if (!button) return;
  const name = button.dataset.name;
  if (!confirm(`Remover o cadastro de ${name}?`)) return;
  button.disabled = true;
  try {
    await api('/api/people/delete', {
      method: 'POST', body: JSON.stringify({ id: Number(button.dataset.id) })
    });
    await refresh();
  } catch (error) {
    alert(error.message);
    button.disabled = false;
  }
};

$('#enrollForm').onsubmit = async event => {
  event.preventDefault();
  try {
    await api('/api/enrollment/start', {
      method: 'POST', body: JSON.stringify({ name: $('#name').value.trim() })
    });
    $('#name').disabled = true;
    $('#start').disabled = true;
    $('#start').textContent = 'Aguardando cartão…';
    $('#enrollMessage').textContent = 'Aproxime o cartão RFID. Cadastro expira em breve.';
    stopEnrollmentPolling();
    enrollTimer = setInterval(async () => {
      try {
        const state = await api('/api/enrollment/status');
        if (state.status !== 'idle') return;
        stopEnrollmentPolling();
        await refresh();
        const messages = {
          duplicate: state.message,
          timeout: 'Tempo limite atingido. Tente novamente.',
          cancelled: 'Cadastro cancelado.',
          enrolled: 'Cadastro concluído com sucesso.'
        };
        $('#enrollMessage').textContent = messages[state.result] || 'Cadastro finalizado.';
        setTimeout(() => { if (modal.open) modal.close(); resetEnrollmentForm(); }, 1400);
      } catch (error) {
        $('#enrollMessage').textContent = error.message;
      }
    }, 600);
  } catch (error) {
    $('#enrollMessage').textContent = error.message;
  }
};

async function cancelEnrollment() {
  stopEnrollmentPolling();
  try { await api('/api/enrollment/cancel', { method: 'POST' }); } catch {}
  if (modal.open) modal.close();
  resetEnrollmentForm();
  $('#enrollMessage').textContent = 'Informe o nome para começar.';
}

refresh();
api('/api/status')
  .then(status => setInterval(refresh, Math.max(500, status.pollIntervalMs || 1500)))
  .catch(() => setInterval(refresh, 1500));
