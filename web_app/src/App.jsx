import React, { useState } from 'react';
import './App.css';

function App() {

  const [alertaActiva, setAlertaActiva] = useState(false);
  const [mostrarAlerta, setMostrarAlerta] = useState(true);
  const [ultimosAvisos, setUltimosAvisos] = useState([
    { hora: 'Hoy, 10:42', evento: 'Caída detectada', estado: 'Pendiente', tipo: 'badge-red' },
    { hora: 'Ayer, 18:15', evento: 'Auxilio por botón', estado: 'Atendido', tipo: 'badge-green' }
  ]);

  return (
    <div className="dashboard-container">
      {/* Header */}
      <header className="header">
        <div className="logo">🛡️ Monitoreo de caídas</div>
        <nav className="nav-links">
          <button className="btn-nav" onClick={() => setAlertaActiva(true)}>Simular Caída</button>
         { /*<button className="btn-nav active">Inicio</button>
         /*  <button className="btn-nav">Contactos</button>*/
         /*  <button className="btn-nav">Ajustes</button>*/}
        </nav>
      </header>
      

      {/* Main Content */}
      <main className="main-content">
        <div className="page-header">
          <h1>Seguimiento de Elena</h1>
          <p>Avisos y estado del dispositivo</p>
        </div>

        {/* Banner de Alerta Activa (se muestra si alertaActiva es true) */}
        {alertaActiva && (
          <div className="alert-banner">
           <h2>⚠️ Caída detectada</h2>
            <p><strong>Hoy, 10:42</strong> - Detección automática</p>
            <p>Alerta enviada a Ana</p>
          <p>Pendiente de atención</p>
  
  <div className="alert-actions">
    <button className="btn-danger" onClick={() => setAlertaActiva(false)}>
  Apagar alarma
</button>
    <span>Apagar solo después de comprobar que Elena está a salvo.</span>
  </div>
</div>
        )}

        {/* Tarjetas de Información */}
        <div className="info-grid">
          <div className="card">
            <h3>Dispositivo</h3>
            <div className="device-status">
              <p><span style={{color: '#48bb78'}}>●</span> Encendido</p>
            </div>
          </div>

          <div className="card contact-card">
            <h3>Contacto de aviso</h3>
            <div className="card-content">
              <div className="avatar">AM</div>
              <div className="contact-details">
                <p><strong>Ana</strong> · Familiar</p>
                <p className="subtext">Notificaciones activas</p>
              </div>
            </div>
          </div>
        </div>

        {/* Historial / Últimos avisos */}
        <div className="history-section">
          <h3>Últimos avisos</h3>
          <ul className="history-list">
            {ultimosAvisos.map((aviso, index) => (
              <li key={index} className="history-item">
                <span className="time">{aviso.hora}</span>
                <span className="event">{aviso.evento}</span>
                <span className={`badge ${aviso.tipo}`}>{aviso.estado}</span>
              </li>
            ))}
          </ul>
        </div>
      </main>
    </div>
  );
}

export default App;