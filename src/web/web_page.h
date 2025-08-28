// ============================================================================
//  Embedded Web Pages (Full Prototype Pages)
//  Source synchronized from /prototype/config.html & /prototype/monitor.html
//  NOTE: Copied verbatim as requested (nav links still point to config.html/monitor.html).
//  If backend routes differ (e.g. "/" and "/monitor"), adjust anchors accordingly later.
//  All prototype scripts (including PROTOTYPE_MODE / mock logic) are retained.
// ============================================================================
#ifndef WEB_PAGE_H
#define WEB_PAGE_H

#include <pgmspace.h>

// Helper macro: gunakan delimiter "HTML" agar aman.

// ============================= CONFIG PAGE ================================
const char CONFIG_PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html><html lang='en'>
<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>
<title>GOES Configurator - Configuration</title>
<style>
 :root { --accent:#0078d4; --accent-hover:#0062ad; --bg:#f2f5f9; --panel:#fff; --border:#d4dbe4; }
 body{font-family:Arial,Helvetica,sans-serif;margin:0;background:var(--bg);color:#222}
 header{background:#1a3d6d;color:#fff;padding:12px 22px;position:sticky;top:0;z-index:1000;display:flex;align-items:center;justify-content:space-between;box-shadow:0 2px 6px rgba(0,0,0,.25)}
 .notif-bar{position:fixed;top:70px;left:50%;transform:translateX(-50%);z-index:1100;background:#fff4c2;color:#664d00;font-size:14px;font-weight:600;padding:12px 24px;display:none;box-shadow:0 4px 12px rgba(0,0,0,.25);border-radius:8px;border:1px solid #e6d700;max-width:90%;text-align:center;animation:slideInDown 0.3s ease-out}
 @keyframes slideInDown{from{opacity:0;transform:translate(-50%,-20px)}to{opacity:1;transform:translate(-50%,0)}}
 h1{margin:0;font-size:19px;letter-spacing:.5px;font-weight:600}
 main{padding:18px;max-width:1220px;margin:0 auto}
 h2{margin:28px 0 12px;font-size:17px;color:#1a3d6d}
 fieldset{border:1px solid var(--border);margin:0 0 18px;padding:14px 16px;border-radius:10px;background:var(--panel)}
 legend{padding:0 6px;font-weight:600;font-size:13px;color:#1a3d6d}
 label.lbl{display:block;margin:0 0 4px;font-weight:600;font-size:11px;letter-spacing:.3px;text-transform:uppercase;color:#425b76}
 input[type=text],input[type=number],select{width:100%;padding:8px 10px;border:1px solid #b8c4d1;border-radius:6px;font-size:14px;background:#fff;box-sizing:border-box}
 button{background:var(--accent);color:#fff;padding:10px 20px;border:none;border-radius:6px;font-size:14px;font-weight:600;cursor:pointer;box-shadow:0 2px 4px rgba(0,0,0,.15);transition:.18s}
 button:hover{background:var(--accent-hover)}
 .grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(210px,1fr));gap:14px}
 .two-col{display:grid;grid-template-columns:repeat(auto-fit,minmax(340px,1fr));gap:18px}
 .muted{font-size:11px;color:#667}
 .status{display:none}
 #liveStatusPanel span { color: #333; }
 #liveStatusPanel strong, #liveStatusPanel code { color: #000; font-weight: 600; }
 #liveStatusPanel.offline{background:#fdeeee;border-color:#f2b8b5}
 #liveStatusPanel.prototype{background:#fff3cd;border:1px solid #ffeaa7;color:#856404}
 .prototype-badge{background:#f39c12;color:#fff;font-size:10px;padding:3px 8px;border-radius:12px;font-weight:700;letter-spacing:.5px;text-transform:uppercase}
 footer{padding:14px 24px;text-align:center;font-size:11px;color:#678;background:#f0f4f8;margin-top:40px;border-top:1px solid #d5dce3}
 pre#logBox{background:#111;color:#0f0;padding:10px;height:260px;overflow:auto;font-size:11px;border-radius:6px;border:1px solid #333}
 .toggles{display:grid;grid-template-columns:repeat(auto-fill,minmax(170px,1fr));gap:10px}
 .toggle-card{display:flex;align-items:center;justify-content:space-between;padding:8px 10px;background:#ffffff;border:1px solid #e1e6ec;border-radius:8px;font-size:11px;font-weight:600;box-shadow:0 1px 2px rgba(0,0,0,.06)}
 .switch{position:relative;display:inline-block;width:42px;height:22px}
 .switch input{opacity:0;width:0;height:0}
 .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#b5bcc5;transition:.3s;border-radius:34px}
 .slider:before{position:absolute;content:"";height:18px;width:18px;left:2px;top:2px;background:#fff;transition:.3s;border-radius:50%;box-shadow:0 1px 3px rgba(0,0,0,.35)}
 .switch input:checked + .slider{background:var(--accent)}
 .switch input:checked + .slider:before{transform:translateX(20px)}
 .group-label{grid-column:1/-1;margin-top:8px;font-size:9.5px;font-weight:700;letter-spacing:.7px;color:#555;text-transform:uppercase}
 .save-bar{padding:10px 0;margin-top:18px;display:flex;gap:12px;align-items:center}
 .tag{background:#1a3d6d;color:#fff;font-size:10px;padding:3px 7px;border-radius:11px;margin-left:10px;letter-spacing:.5px}
 #ioa-dynamic fieldset{margin-bottom:14px}
 #ioa-dynamic .ioa-row{display:grid;grid-template-columns:150px 1fr;align-items:center;gap:8px;margin:4px 0}
 #ioa-dynamic label{font-size:12px;font-weight:600;color:#244}
 .notif-bar.error{background:#ff4757;color:#fff;border-color:#ff3838}
 .notif-bar.info{background:#2ed573;color:#fff;border-color:#20bf6b}
 .notif-bar.success{background:#2ed573;color:#fff;border-color:#20bf6b}
 .notif-bar.warning{background:#ffa502;color:#fff;border-color:#ff9500}
</style></head>
<body><header><h1>GOES RTU Configuration</h1><span class='tag'>v2.x-proto</span>
<nav style='margin-left:auto'>
	<a href='/' style='color:#fff;text-decoration:none;margin-right:15px;font-weight:600'>Configuration</a>
	<a href='/monitor' style='color:#ccc;text-decoration:none;font-weight:600'>Monitoring</a>
</nav>
</header>
<div id='liveStatusPanel' style='display:flex;flex-wrap:wrap;align-items:center;gap:10px 18px;padding:8px 22px;background:#e9f2ff;border-bottom:1px solid #c4d9f2;font-size:11.5px'>
 <span class='prototype-badge'>Prototype Mode</span>
 <span id='ls_conn'>Polling...</span>
 <span>| Comm: <strong id='ls_comm'>-</strong></span>
 <span>| IEC104 Tx:<span id='ls_iec104_tx'>-</span></span>
 <span>| IEC104 Rx:<span id='ls_iec104_rx'>-</span></span>
 <span>| IEC104 Last(ms):<span id='ls_iec104_last'>-</span></span>
 <span>| Uptime: <span id='ls_uptime'>-</span></span>
 <span>| Heap: <span id='ls_heap'>-</span></span>
 <span>| Stack(main): <span id='ls_stack'>-</span></span>
 <span>| Stack(comm): <span id='ls_stack_comm'>-</span></span>
 <span>| Stack(iec104): <span id='ls_stack_iec104'>-</span></span>
 <span>| Stack(hal): <span id='ls_stack_hal'>-</span></span>
 <span>| Rev: <code id='ls_rev'>-</code></span>
</div>
<div id='rtuClockBar' style='display:flex;align-items:center;gap:14px;padding:10px 22px;background:#102a46;color:#fff;font-size:14px;font-weight:600;border-bottom:2px solid #0d2238'>
	<div style='font-size:12px;letter-spacing:.5px;text-transform:uppercase;opacity:.85'>RTU Clock</div>
	<div style='font-family:monospace;font-size:16px' id='rtu_time_full'>--</div>
	<div style='font-size:11px;background:#1f4d80;padding:3px 8px;border-radius:12px' id='rtu_time_source'>Source: -</div>
	<div style='margin-left:auto;font-size:11px;opacity:.8'>Updated <span id='rtu_time_updated'>-</span></div>
</div>
<div id='notif' class='notif-bar'></div><main>
<section id='device-section'><h2>Device Configuration</h2>
<form id='device-form'><div class='two-col'>
<!-- System fieldset (top) adjustments: remove embedded restart button, add time source select -->
<fieldset><legend>System</legend>
 <div class='grid' style='grid-template-columns:repeat(auto-fill,minmax(180px,1fr));gap:12px'>
		<div style='display:flex;align-items:center;justify-content:space-between' class='toggle-card'>
			 <span>SOE</span>
			 <label class='switch'><input type='checkbox' id='feat_soe'><span class='slider'></span></label>
		</div>
		<div>
			 <label class='lbl' for='soe_buffer'>SOE Buffer</label>
			 <input type='number' id='soe_buffer' name='soe_buffer' placeholder='64' min='1'>
			 <div class='muted'>Ukuran penampung event.</div>
		</div>
		<div>
			 <label class='lbl' for='time_source'>Time Source</label>
			 <select id='time_source' name='time_source'>
					<option value='ntp'>NTP</option>
					<option value='iec104'>IEC104 TI103</option>
			 </select>
			 <div class='muted'>Sumber update RTC.</div>
		</div>
		<div>
			 <label class='lbl' for='time_sync_s'>Time Sync (s)</label>
			 <input type='number' id='time_sync_s' name='time_sync_s' placeholder='3600' min='10'>
			 <div class='muted'>Interval sync.</div>
		</div>
		<div>
			 <label class='lbl' for='heartbeat_s'>Heartbeat (s)</label>
			 <input type='number' id='heartbeat_s' name='heartbeat_s' placeholder='30' min='1'>
			 <div class='muted'>Interval heartbeat.</div>
		</div>
 </div>
</fieldset>
<fieldset><legend>Communication</legend>
 <div class='grid'>
	 <div><label class='lbl' for='comm_method'>Mode</label><select id='comm_method' name='comm_method'><option value='wifi'>WiFi</option><option value='ethernet'>Ethernet</option><option value='modem'>Modem Cellular</option></select></div>
	 <div id='wifi-settings' style='display:none'>
			<label class='lbl' for='wifi_ssid'>WiFi SSID</label><input type='text' id='wifi_ssid' name='wifi_ssid' placeholder='Your_WiFi_Network'>
			<label class='lbl' for='wifi_password'>WiFi Password</label><input type='password' id='wifi_password' name='wifi_password' placeholder='WiFi Password'>
	 </div>
	 <div id='modem-settings'>
			<label class='lbl' for='modem_type'>Modem Type</label><select id='modem_type' name='modem_type'><option value='sim800l'>SIM800L</option><option value='sim7600ce'>SIM7600CE</option><option value='quectel_ec25k'>Quectel EC25-K</option></select>
			<label class='lbl' for='apn'>APN</label><input type='text' id='apn' name='apn' placeholder='APN provider'>
	 </div>
	 <div id='ethernet-settings' style='display:none'>
			<label class='lbl' for='eth_mac'>Ethernet MAC</label><input type='text' id='eth_mac' name='eth_mac' placeholder='AA:BB:CC:DD:EE:FF'>
			<label class='lbl' for='eth_ip'>Static IP</label><input type='text' id='eth_ip' name='eth_ip' placeholder='192.168.1.50'>
			<label class='lbl' for='eth_gateway'>Gateway IP</label><input type='text' id='eth_gateway' name='eth_gateway' placeholder='192.168.1.1'>
			<label class='lbl' for='eth_netmask'>Netmask</label><input type='text' id='eth_netmask' name='eth_netmask' placeholder='255.255.255.0'>
	 </div>
 </div>
</fieldset>
<fieldset><legend>IEC 60870-5-104</legend>
 <div class='grid'>
		<div>
			 <label class='lbl' for='iec_common_addr'>Common Address</label>
			 <input type='number' id='iec_common_addr' name='iec_common_addr' placeholder='1' min='0' max='65535'>
		</div>
		<div>
			 <label class='lbl' for='iec_k'>K (Send Window)</label>
			 <input type='number' id='iec_k' name='iec_k' placeholder='12' min='1' max='255'>
			 <div class='muted'>Prototype.</div>
		</div>
		<div>
			 <label class='lbl' for='iec_w'>W (Recv Window)</label>
			 <input type='number' id='iec_w' name='iec_w' placeholder='8' min='1' max='255'>
			 <div class='muted'>Window penerimaan (baru UI).</div>
		</div>
 </div>
</fieldset>
<fieldset><legend>Main Features</legend>
 <div class='toggles' id='mainFeatures'>
	 <div class='toggle-card'><span>GFD</span><label class='switch'><input type='checkbox' id='feat_gfd'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>Supply</span><label class='switch'><input type='checkbox' id='feat_supply'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>CB 1</span><label class='switch'><input type='checkbox' id='feat_cb1'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>CB 2</span><label class='switch'><input type='checkbox' id='feat_cb2'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>CB 3</span><label class='switch'><input type='checkbox' id='feat_cb3'><span class='slider'></span></label></div>
 </div>
 <div id='ioa-main' style='margin-top:10px'></div>
</fieldset>
<fieldset><legend>Extended Features</legend>
 <div class='toggles' id='extFeatures'>
	 <div class='toggle-card'><span>Door</span><label class='switch'><input type='checkbox' id='feat_door'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>Temperature</span><label class='switch'><input type='checkbox' id='feat_temp'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>Humidity</span><label class='switch'><input type='checkbox' id='feat_humidity'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DI 1</span><label class='switch'><input type='checkbox' id='feat_di1'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DI 2</span><label class='switch'><input type='checkbox' id='feat_di2'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DI 3</span><label class='switch'><input type='checkbox' id='feat_di3'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DI 4</span><label class='switch'><input type='checkbox' id='feat_di4'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DI 5</span><label class='switch'><input type='checkbox' id='feat_di5'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DO 1</span><label class='switch'><input type='checkbox' id='feat_do1'><span class='slider'></span></label></div>
	 <div class='toggle-card'><span>DO 2</span><label class='switch'><input type='checkbox' id='feat_do2'><span class='slider'></span></label></div>
 </div>
 <div id='ioa-ext' style='margin-top:10px'></div>
</fieldset>
<fieldset><legend>Modbus</legend>
 <div class='toggle-card' style='max-width:220px'><span>Enable Modbus</span><label class='switch'><input type='checkbox' id='feat_modbus'><span class='slider'></span></label></div>
 <div style='margin-top:10px' class='grid' id='modbus-core'>
	 <div>
		 <label class='lbl' for='modbus_mode'>Mode</label>
		 <select id='modbus_mode' name='modbus_mode'>
			 <option value='rtu'>RTU</option>
			 <option value='tcp'>TCP</option>
		 </select>
	 </div>
	 <div>
		 <label class='lbl' for='modbus_baud'>Baud</label>
		 <select id='modbus_baud' name='modbus_baud'>
			 <option>9600</option><option>19200</option><option selected>115200</option>
		 </select>
	 </div>
	 <div>
		 <label class='lbl' for='modbus_parity'>Parity</label>
		 <select id='modbus_parity' name='modbus_parity'>
			 <option value='N'>None</option><option value='E'>Even</option><option value='O'>Odd</option>
		 </select>
	 </div>
	 <div>
		 <label class='lbl' for='modbus_poll_ms'>Global Poll (ms)</label>
		 <input type='number' id='modbus_poll_ms' name='modbus_poll_ms' placeholder='1000' min='50'>
	 </div>
 </div>
 <div id='modbus-slaves-section' style='margin-top:14px'>
	 <div style='display:flex;align-items:center;gap:10px;margin-bottom:8px'><strong style='font-size:13px'>Slaves & Registers</strong><button type='button' id='add-modbus-slave'>Add Slave</button></div>
	 <div id='modbus-slaves'></div>
 </div>
</fieldset>
<p class='muted' style='margin-top:6px'>Set fitur & IOA. Modbus daftar dinamis + param inti.</p>
 </div>
 <div id='ioa-dynamic' style='margin-top:28px'></div>
 <div class='save-bar' style='display:flex;gap:12px;align-items:center'>
	<button type='button' id='btnRestart' style='background:#d9534f'>Restart RTU</button>
	<button type='submit'>Save Configuration</button>
</div></form></section>
<div id='status' class='status'></div>
</main><footer>GOES &copy; 2025</footer>
<script>
// ===== PROTOTYPE MODE - MOCK DATA & LOCAL STORAGE =====
const PROTOTYPE_MODE = true;
// Mock API responses
const MOCK_STATUS = {
	uptime: 1234,
	rtu_time: (function(){const d=new Date();return d.getFullYear()+ '-' + String(d.getMonth()+1).padStart(2,'0') + '-' + String(d.getDate()).padStart(2,'0') + ' ' + String(d.getHours()).padStart(2,'0') + ':' + String(d.getMinutes()).padStart(2,'0') + ':' + String(d.getSeconds()).padStart(2,'0') + '.' + String(d.getMilliseconds()).padStart(3,'0');})(),
	comm_method: 'wifi',
	wifi_ssid: 'GOES_Network',
	wifi_rssi: -45,
	heap_free: 150000,
	main_task_stack_free_words: 2048,
	comm_task_stack_free_words: 1024,
	iec104_task_stack_free_words: 1024,
	hal_task_stack_free_words: 1024,
	iec104_tx: 42,
	iec104_rx: 38,
	iec104_last_ms: 1500,
	iec104_connected: true,
	heartbeat_ms: 30000,
	rev: 'proto-v1.0'
};

const MOCK_CONFIG = {
	device: {
		comm_method: 'wifi',
		wifi_ssid: 'GOES_Network',
		wifi_password: 'password123',
		apn: 'internet',
		modem_type: 'sim7600ce',
		eth_mac: '',
		eth_ip: '',
		soe_buffer: 64,
		iec_common_addr: 1,
		iec_k: 12,
		iec_w: 8,
		modbus_mode: 'rtu',
		modbus_baud: 9600,
		modbus_parity: 'none',
		modbus_poll_ms: 1000,
		time_sync_s: 3600,
		heartbeat_s: 30,
		time_source: 'ntp',
		feat_gfd: 1,
		feat_supply: 1,
		feat_door: 0,
		feat_temp: 1,
		feat_humidity: 1,
		feat_cb1: 1,
		feat_cb2: 1,
		feat_cb3: 0,
		feat_modbus: 1,
		feat_di1: 1,
		feat_di2: 1,
		feat_di3: 0,
		feat_di4: 0,
		feat_di5: 0,
		feat_do1: 1,
		feat_do2: 1,
		feat_soe: 1
	},
	ioa: {
		di_gfd: 1001,
		di_supply_status: 1002,
		remote_cb_1: 2001,
		status_cb_1: 3001,
		command_cb_1: 4001,
		remote_cb_2: 2002,
		status_cb_2: 3002,
		command_cb_2: 4002,
		ai_temperature: 5001,
		ai_humidity: 5002,
		di1: 1011,
		di2: 1012,
		do1: 6001,
		do2: 6002,
		modbus_list: [
			{name: 'Voltage', ioa: 7001},
			{name: 'Current', ioa: 7002}
		]
	}
};

const MOCK_LOGS = {
	lines: [
		'[SYSTEM][T+123456] GOES RTU Started',
		'[WiFi][T+123500] Connected to network',
		'[IEC104][T+123600] Connection established',
		'[HAL][T+123700] All sensors initialized',
		'[MODBUS][T+123800] Slave devices detected',
		'[COMM][T+123900] Communication active'
	]
};

function mockFetch(url, options = {}) { return new Promise((resolve) => { setTimeout(() => { if (url.includes('/api/status')) { resolve({ ok: true, json: () => Promise.resolve(MOCK_STATUS) }); } else if (url.includes('/api/config2') || url.includes('/api/config')) { resolve({ ok: true, json: () => Promise.resolve(MOCK_CONFIG) }); } else if (url.includes('/api/logs')) { resolve({ ok: true, json: () => Promise.resolve(MOCK_LOGS) }); } else if (url.includes('/api/config/device') && options.method === 'POST') { const formData = new URLSearchParams(options.body); const config = Object.fromEntries(formData); localStorage.setItem('prototype_device_config', JSON.stringify(config)); resolve({ ok: true, text: () => Promise.resolve('Configuration saved (prototype mode)') }); } else if (url.includes('/api/config/ioa') && options.method === 'POST') { const config = JSON.parse(options.body || '{}'); localStorage.setItem('prototype_ioa_config', JSON.stringify(config)); resolve({ ok: true, text: () => Promise.resolve('IOA Configuration saved (prototype mode)') }); } else { resolve({ ok: true, text: () => Promise.resolve('Mock response') }); } }, 200); }); }
if (PROTOTYPE_MODE) { window.fetch = mockFetch; }
let lastStatus=null,initialRev=null,expectingRestart=false,pollIntervalMs=5000;let logTimer=null;let soeTimer=null;let currentIoa={};
document.addEventListener('DOMContentLoaded',()=>{loadConfig();const commMethod=byId('comm_method');if(commMethod){commMethod.addEventListener('change',toggleCommSections);commMethod.addEventListener('change',saveConfigState);}const deviceForm=byId('device-form');if(deviceForm){deviceForm.addEventListener('submit',onUnifiedSubmit);}startStatusPolling();initLogs();initSOE();featureListeners();saveConfigState();document.getElementById('device-form').addEventListener('change', saveConfigState);setupDynamicButtons();});
function saveConfigState(){const featureConfig={};['feat_gfd','feat_supply','feat_door','feat_temp','feat_humidity','feat_cb1','feat_cb2','feat_cb3','feat_modbus','feat_di1','feat_di2','feat_di3','feat_di4','feat_di5','feat_do1','feat_do2','feat_soe'].forEach(id=>{const el=byId(id);if(el){featureConfig[id]=el.checked;}});featureConfig.comm_method=byId('comm_method').value;featureConfig.modem_type=byId('modem_type').value;featureConfig.wifi_ssid=byId('wifi_ssid').value;featureConfig.eth_mac=byId('eth_mac').value;featureConfig.eth_ip=byId('eth_ip').value;featureConfig.eth_gateway=byId('eth_gateway').value;featureConfig.eth_netmask=byId('eth_netmask').value;localStorage.setItem('goesConfigState',JSON.stringify(featureConfig));}
function featureListeners(){['feat_gfd','feat_supply','feat_door','feat_temp','feat_humidity','feat_cb1','feat_cb2','feat_cb3','feat_modbus','feat_di1','feat_di2','feat_di3','feat_di4','feat_di5','feat_do1','feat_do2'].forEach(id=>{const el=byId(id);if(el)el.addEventListener('change',()=>{renderGroupIoa();toggleModbusVisibility();applyAutoDisableIOA();saveConfigState();});});}
function setupDynamicButtons(){const am=byId('add-modbus');if(am)am.addEventListener('click',()=>addListRow('modbus'));const ams=byId('add-modbus-slave');if(ams)ams.addEventListener('click',()=>addModbusSlaveRowNew());}
function initLogs(){const b=byId('btnRefreshLogs');if(b)b.addEventListener('click',loadLogs);const a=byId('autoLogs');if(a)a.addEventListener('change',()=>{if(a.checked&&byId('log_enable')?.checked){scheduleLogs();}else if(logTimer){clearTimeout(logTimer);}});const e=byId('log_enable');if(e)e.addEventListener('change',()=>{if(!e.checked){if(logTimer)clearTimeout(logTimer);const box=byId('logBox');if(box)box.textContent='Logs disabled';}else if(byId('autoLogs')?.checked){scheduleLogs();loadLogs();}});const s=byId('autoScrollLogs');if(s&&!s.hasAttribute('data-initialized')){s.setAttribute('data-initialized','true');}if(e?.checked){scheduleLogs();loadLogs();}}
function scheduleLogs(){if(logTimer)clearTimeout(logTimer);const a=byId('autoLogs');const e=byId('log_enable');if(!a||!a.checked||!e||!e.checked)return;logTimer=setTimeout(()=>{loadLogs();scheduleLogs();},2000);} 
function loadLogs(){const e=byId('log_enable');if(!e||!e.checked)return;fetch('/api/logs').then(r=>r.json()).then(j=>{const box=byId('logBox');if(!box)return;const wasAtBottom=box.scrollTop>=(box.scrollHeight-box.clientHeight-5);box.dataset.raw=(j.lines||[]).join('\n');box.textContent=box.dataset.raw;filterLogBox();const autoScroll=byId('autoScrollLogs');if(autoScroll?.checked&&(wasAtBottom||!box.textContent.trim())){setTimeout(()=>{box.scrollTop=box.scrollHeight;},50);}}).catch(()=>{});}
document.addEventListener('DOMContentLoaded',()=>{const btnSet=byId('btnSetLogCap');if(btnSet)btnSet.addEventListener('click',()=>{const v=parseInt(byId('logCap').value,10);if(isNaN(v)){showStatus('Isi log cap',true);return;}fetch('/api/logs/resize',{method:'POST',body:new URLSearchParams({cap:String(v)})}).then(r=>r.text()).then(t=>showStatus(t,/resized/i.test(t)?false:true));});const btnClr=byId('btnClearLogs');if(btnClr)btnClr.addEventListener('click',()=>{fetch('/api/logs/clear',{method:'POST'}).then(r=>r.text()).then(t=>{showStatus(t,false);loadLogs();});});});
function initSOE(){const r=byId('btnRefreshSoe');if(r)r.addEventListener('click',loadSOE);const c=byId('btnClearSoe');if(c)c.addEventListener('click',()=>{fetch('/api/soe/clear',{method:'POST'}).then(r=>r.text()).then(t=>{showStatus(t,false);loadSOE();});});const a=byId('autoSoe');if(a)a.addEventListener('change',()=>{if(a.checked){scheduleSOE();}else if(soeTimer)clearTimeout(soeTimer);});scheduleSOE();loadSOE();}
function scheduleSOE(){if(soeTimer)clearTimeout(soeTimer);const a=byId('autoSoe');if(!a||!a.checked)return;soeTimer=setTimeout(()=>{loadSOE();scheduleSOE();},3000);} 
function loadSOE(){fetch('/api/soe').then(r=>r.json()).then(j=>{const body=byId('soeBody');const sum=byId('soeSummary');if(sum)sum.textContent=`${j.count||0}/${j.cap||0} overflow=${j.overflow?'Y':'N'}`;if(body){body.innerHTML='';(j.events||[]).slice(-200).reverse().forEach(ev=>{const tr=document.createElement('tr');tr.innerHTML=`<td style='padding:2px 4px;border-bottom:1px solid #eee'>${ev.seq}</td><td style='padding:2px 4px;border-bottom:1px solid #eee'>${ev.ms}</td><td style='padding:2px 4px;border-bottom:1px solid #eee'>${ev.ioa}</td><td style='padding:2px 4px;border-bottom:1px solid #eee'>${ev.v}</td>`;body.appendChild(tr);});}}).catch(()=>{});}
function loadConfig(){if(PROTOTYPE_MODE){const savedDevice=localStorage.getItem('prototype_device_config');const savedIoa=localStorage.getItem('prototype_ioa_config');if(savedDevice||savedIoa){const deviceConfig=savedDevice?JSON.parse(savedDevice):MOCK_CONFIG.device;const ioaConfig=savedIoa?JSON.parse(savedIoa):MOCK_CONFIG.ioa;applyDeviceConfig(deviceConfig);applyIoaConfig(ioaConfig);return;}}fetch('/api/config2').then(r=>{if(!r.ok)throw new Error('Config2 failed');return r.json();}).then(d=>{const dv=(d.device&&typeof d.device==='object')?d.device:parseJSON(d.device||'{}');const io=(d.ioa&&typeof d.ioa==='object')?d.ioa:parseJSON(d.ioa||'{}');applyDeviceConfig(dv);applyIoaConfig(io);}).catch(()=>{fetch('/api/config').then(r=>r.json()).then(d=>{const dv=(d.device&&typeof d.device==='object')?d.device:parseJSON(d.device||'{}');const io=(d.ioa&&typeof d.ioa==='object')?d.ioa:parseJSON(d.ioa||'{}');applyDeviceConfig(dv);applyIoaConfig(io);}).catch(()=>{});});}
function parseJSON(s){try{return JSON.parse(s);}catch(e){return{};}}
function applyDeviceConfig(dv){setVal('comm_method',dv.comm_method||'wifi');['apn','modem_type','eth_mac','eth_ip','eth_gateway','eth_netmask','wifi_ssid','wifi_password','soe_buffer','iec_common_addr','iec_k','iec_w','modbus_mode','modbus_baud','modbus_parity','modbus_poll_ms','time_sync_s','heartbeat_s','time_source'].forEach(k=>setVal(k,dv[k]||''));const feats=['feat_gfd','feat_supply','feat_door','feat_temp','feat_humidity','feat_cb1','feat_cb2','feat_cb3','feat_modbus','feat_di1','feat_di2','feat_di3','feat_di4','feat_di5','feat_do1','feat_do2','feat_soe'];feats.forEach(id=>setToggle(id,dv[id]!==0));if(!byId('comm_method').getAttribute('data-original'))byId('comm_method').setAttribute('data-original',dv.comm_method||'wifi');toggleCommSections();maybeHideEthernet(dv);renderGroupIoa();}
function maybeHideEthernet(dv){if(dv.feat_ethernet===0){const opt=[...byId('comm_method').options].find(o=>o.value==='ethernet');if(opt)opt.style.display='none';if(byId('comm_method').value==='ethernet'){byId('comm_method').value='modem';}}}
function applyIoaConfig(io){currentIoa=io||{};if((currentIoa.mb_reg1||currentIoa.mb_reg2)&&(!currentIoa.modbus_list||!currentIoa.modbus_list.length)){currentIoa.modbus_list=[];if(currentIoa.mb_reg1)currentIoa.modbus_list.push({name:'Reg1',ioa:currentIoa.mb_reg1});if(currentIoa.mb_reg2)currentIoa.modbus_list.push({name:'Reg2',ioa:currentIoa.mb_reg2});}renderGroupIoa();toggleModbusVisibility();renderDynamicLists();applyAutoDisableIOA();}
function onUnifiedSubmit(e){e.preventDefault();applyAutoDisableIOA();const dev=formToJSON(e.target);['feat_gfd','feat_supply','feat_door','feat_temp','feat_humidity','feat_cb1','feat_cb2','feat_cb3','feat_modbus','feat_di1','feat_di2','feat_di3','feat_di4','feat_di5','feat_do1','feat_do2','feat_soe'].forEach(id=>dev[id]=byId(id).checked?1:0);const oldMethod=byId('comm_method').getAttribute('data-original')||'';if(oldMethod&&oldMethod!==dev.comm_method)expectingRestart=true;const ioa={};document.querySelectorAll('[data-ioa]')?.forEach(inp=>{const k=inp.name;const v=inp.value.trim();if(v!==''&&!isNaN(v))ioa[k]=parseInt(v,10);});if(byId('feat_modbus')?.checked){const regs=collectModbusRegisters();if(regs.length)ioa.modbus_list=regs;const slaves=collectModbusSlaves();if(slaves.length)ioa.modbus_slaves=slaves;}showNotif('Saving configuration...',false);saveConfig('/api/config/device',dev,(ok1,msg1)=>{if(!ok1){showNotif('Device save failed: '+msg1,true);return;}saveConfig('/api/config/ioa',ioa,(ok2,msg2)=>{if(!ok2){showNotif('IOA save failed: '+msg2,true);return;}showNotif('Configuration saved'+(expectingRestart?' (restarting...)':'')+'.',false);});});}
function byId(id){return document.getElementById(id);}function setVal(id,v){const el=byId(id);if(el)el.value=v;}function setToggle(id,on){const el=byId(id);if(el)el.checked=!!on;}
function toggleCommSections(){const method=byId('comm_method').value;byId('wifi-settings').style.display=method==='wifi'?'block':'none';byId('modem-settings').style.display=method==='modem'?'block':'none';byId('ethernet-settings').style.display=method==='ethernet'?'block':'none';}
function formToJSON(f){return Object.fromEntries(new FormData(f).entries());}
function saveConfig(url,data,cb){fetch(url,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.text().then(t=>({ok:r.ok,text:t}))).then(o=>{if(/Restarting/i.test(o.text))expectingRestart=true;if(cb)cb(o.ok,o.text);}).catch(e=>{if(cb)cb(false,String(e));});}
document.addEventListener('DOMContentLoaded',()=>{const br=byId('btnRestart');if(br)br.addEventListener('click',()=>{if(!confirm('Yakin restart RTU sekarang?'))return;showStatus('Restarting...');fetch('/api/restart',{method:'POST'}).then(r=>r.text()).then(t=>{showStatus(t||'Restart issued');expectingRestart=true;}).catch(()=>showStatus('Gagal kirim restart',true));});});
function showStatus(msg,err){showNotif(msg,err?'error':'info');}
function startStatusPolling(){pollStatus();setInterval(pollStatus,pollIntervalMs);} 
function pollStatus(){fetch('/api/status').then(r=>{if(!r.ok)throw new Error('HTTP '+r.status);return r.json();}).then(s=>{updateLiveStatus(s);updateRtuClockBarConfig(s.rtu_time,s.time_source||'RTC');if(lastStatus){if(s.rev!==lastStatus.rev)showStatus('Revision: '+s.rev,false);if(lastStatus.uptime>=5&&s.uptime+3<lastStatus.uptime)showStatus('Device restarted (reason: '+(s.reset_reason||'?')+')',false);}lastStatus=s;if(initialRev===null)initialRev=s.rev;}).catch(()=>{setOffline();});}
function updateLiveStatus(s){const panel=byId('liveStatusPanel');panel.classList.remove('offline');if(PROTOTYPE_MODE){panel.classList.add('prototype');}byId('ls_conn').textContent='Online';byId('ls_comm').textContent=s.comm_method||'-';byId('ls_uptime').textContent=formatUptime(s.uptime||0);byId('ls_heap').textContent=s.heap_free||'-';byId('ls_stack').textContent=s.main_task_stack_free_words||'-';byId('ls_stack_comm').textContent=s.comm_task_stack_free_words||'-';byId('ls_stack_iec104').textContent=s.iec104_task_stack_free_words||'-';byId('ls_stack_hal').textContent=s.hal_task_stack_free_words||'-';byId('ls_rev').textContent=s.rev||'-';if(s.iec104_tx!==undefined)byId('ls_iec104_tx').textContent=s.iec104_tx;if(s.iec104_rx!==undefined)byId('ls_iec104_rx').textContent=s.iec104_rx;if(s.iec104_last_ms!==undefined)byId('ls_iec104_last').textContent=s.iec104_last_ms;if(expectingRestart&&lastStatus&&s.uptime<8){showNotif('Restart complete (Comm='+s.comm_method+')',false);expectingRestart=false;}if(s.iec104_connected!==undefined){let el=document.getElementById('iec104_link');if(!el){const span=document.createElement('span');span.id='iec104_link';panel.appendChild(span);el=span;}el.textContent='| IEC104:'+(s.iec104_connected?'ON':'OFF');}if(s.heartbeat_ms!==undefined){let el=document.getElementById('hb_status');if(!el){const span=document.createElement('span');span.id='hb_status';panel.appendChild(span);el=span;}el.textContent='| HB(ms):'+s.heartbeat_ms;}}
function setOffline(){const p=byId('liveStatusPanel');p.classList.add('offline');['ls_conn','ls_comm','ls_uptime','ls_rev','ls_heap','ls_stack','ls_stack_comm','ls_stack_iec104','ls_stack_hal'].forEach(id=>{const el=byId(id);if(el)el.textContent='-';});byId('ls_conn').textContent='Offline';}
function updateRtuClockBarConfig(ts,source){const full=byId('rtu_time_full');if(full)full.textContent=ts||'--';const src=byId('rtu_time_source');if(src)src.textContent='Source: '+(source||'RTC');const upd=byId('rtu_time_updated');if(upd)upd.textContent=new Date().toLocaleTimeString();}
function formatUptime(sec){const h=Math.floor(sec/3600),m=Math.floor((sec%3600)/60),s=sec%60;return(h>0?h+'h ':'')+(m>0||h>0?m+'m ':'')+s+'s';}
function renderGroupIoa(){const feat=id=>byId(id)&&byId(id).checked;const main=byId('ioa-main');const ext=byId('ioa-ext');if(!main||!ext)return;const add=(arr,label,key)=>{const stored=currentIoa[key];const val=(stored===0)?0:(stored||'');arr.push(`<div class='ioa-row'><label>${label}</label><input data-ioa name='${key}' type='number' value='${val}' placeholder='IOA'></div>`);};let m=[];if(feat('feat_gfd'))add(m,'GFD','di_gfd');if(feat('feat_supply'))add(m,'Supply','di_supply_status');if(feat('feat_cb1')){add(m,'Remote 1','remote_cb_1');add(m,'CB1 Status','status_cb_1');add(m,'CB1 Command','command_cb_1');}if(feat('feat_cb2')){add(m,'Remote 2','remote_cb_2');add(m,'CB2 Status','status_cb_2');add(m,'CB2 Command','command_cb_2');}if(feat('feat_cb3')){add(m,'Remote 3','remote_cb_3');add(m,'CB3 Status','status_cb_3');add(m,'CB3 Command','command_cb_3');}main.innerHTML=m.join('');let e=[];if(feat('feat_door'))add(e,'Door','di_door');if(feat('feat_temp'))add(e,'Temperature','ai_temperature');if(feat('feat_humidity'))add(e,'Humidity','ai_humidity');if(feat('feat_di1'))add(e,'DI 1','di1');if(feat('feat_di2'))add(e,'DI 2','di2');if(feat('feat_di3'))add(e,'DI 3','di3');if(feat('feat_di4'))add(e,'DI 4','di4');if(feat('feat_di5'))add(e,'DI 5','di5');if(feat('feat_do1'))add(e,'DO 1','do1');if(feat('feat_do2'))add(e,'DO 2','do2');ext.innerHTML=e.join('');}
function toggleModbusVisibility(){const enabled=byId('feat_modbus')?.checked;const core=byId('modbus-core');const slaves=byId('modbus-slaves-section');if(core)core.style.display=enabled?'grid':'none';if(slaves)slaves.style.display=enabled?'block':'none';}
function addListRow(type,obj){if(type==='modbus'){const container=byId('modbus-items');if(!container)return;const nameVal=obj&&obj.name?obj.name:'';const ioaVal=obj&&obj.ioa?obj.ioa:'';const div=document.createElement('div');div.className='modbus-row';div.style.cssText='margin:3px 0;padding:4px 6px;border:1px solid #ccc;border-radius:6px;display:flex;gap:6px;align-items:center;background:#fff';div.innerHTML=`<input type='text' placeholder='Name' value='${nameVal}' class='modbus-name' style='width:140px'> <input type='number' placeholder='IOA' value='${ioaVal}' class='modbus-ioa' style='width:90px'> <button type='button' class='mb-reg-del'>✕</button>`;container.appendChild(div);div.querySelector('.mb-reg-del').addEventListener('click',()=>div.remove());}}
function collectModbusRegisters(){return[...document.querySelectorAll('#modbus-items .modbus-row')].map(r=>{const name=r.querySelector('.modbus-name').value.trim();const ioa=parseInt(r.querySelector('.modbus-ioa').value.trim(),10);return(name&&!isNaN(ioa))?{name,ioa}:null;}).filter(x=>x);} 
function addModbusSlaveRowNew(obj){const container=byId('modbus-slaves');if(!container)return;const nameVal=obj&&obj.name?obj.name:'';const idVal=obj&&obj.id?obj.id:'';const pollVal=obj&&obj.poll?obj.poll:'';const addrVal=obj&&obj.address?obj.address:'';const registers=obj&&obj.registers?obj.registers:[];const div=document.createElement('div');div.className='modbus-slave-group';div.style.cssText='margin:8px 0;padding:8px;border:2px solid #ddd;border-radius:8px;background:#f9f9f9';div.innerHTML=`<div class='slave-header' style='display:flex;gap:6px;align-items:center;margin-bottom:8px;padding:4px;background:#fff;border-radius:6px;border:1px solid #ccc'><input type='number' min='1' max='247' placeholder='Slave ID' value='${idVal}' class='mb-slave-id' style='width:80px'> <input type='text' placeholder='Device Name' value='${nameVal}' class='mb-slave-name' style='width:140px'> <input type='number' placeholder='Poll ms' value='${pollVal}' class='mb-slave-poll' style='width:80px'> <button type='button' class='mb-slave-del' style='background:#dc3545;color:#fff;border:none;padding:4px 8px;border-radius:4px;cursor:pointer'>Delete Slave</button></div><div class='registers-section'><div style='display:flex;align-items:center;gap:8px;margin-bottom:6px'><strong style='font-size:12px;color:#666'>Registers:</strong><button type='button' class='add-register' style='background:#28a745;color:#fff;border:none;padding:3px 8px;border-radius:4px;font-size:11px;cursor:pointer'>Add Register</button></div><div class='registers-container'></div></div>`;container.appendChild(div);if(registers.length>0){registers.forEach(reg=>addRegisterToSlave(div,reg));}div.querySelector('.mb-slave-del').addEventListener('click',()=>div.remove());div.querySelector('.add-register').addEventListener('click',()=>addRegisterToSlave(div));}
function addRegisterToSlave(slaveDiv,regObj=null){const container=slaveDiv.querySelector('.registers-container');const nameVal=regObj&&regObj.name?regObj.name:'';const typeVal=regObj&&regObj.type?regObj.type:'holding';const addrVal=regObj&&regObj.address?regObj.address:'';const regDiv=document.createElement('div');regDiv.className='register-row';regDiv.style.cssText='margin:3px 0;padding:4px 6px;border:1px solid #bbb;border-radius:4px;display:flex;gap:4px;align-items:center;background:#fff;font-size:12px';regDiv.innerHTML=`<input type='text' placeholder='Register Name' value='${nameVal}' class='reg-name' style='width:120px;font-size:12px;padding:3px'><select class='reg-type' style='width:90px;font-size:12px;padding:3px'><option value='holding' ${typeVal==='holding'?'selected':''}>Holding</option><option value='input' ${typeVal==='input'?'selected':''}>Input</option><option value='coil' ${typeVal==='coil'?'selected':''}>Coil</option><option value='discrete' ${typeVal==='discrete'?'selected':''}>Discrete</option></select><input type='number' placeholder='Address' value='${addrVal}' class='reg-addr' style='width:80px;font-size:12px;padding:3px'><button type='button' class='reg-del' style='background:#dc3545;color:#fff;border:none;padding:2px 6px;border-radius:3px;font-size:10px;cursor:pointer'>✕</button>`;container.appendChild(regDiv);regDiv.querySelector('.reg-del').addEventListener('click',()=>regDiv.remove());}
function collectModbusSlaves(){return[...document.querySelectorAll('#modbus-slaves .modbus-slave-group')].map(slaveDiv=>{const id=parseInt(slaveDiv.querySelector('.mb-slave-id').value,10);const name=slaveDiv.querySelector('.mb-slave-name').value.trim();const poll=parseInt(slaveDiv.querySelector('.mb-slave-poll').value,10);const registers=[...slaveDiv.querySelectorAll('.register-row')].map(regDiv=>{const regName=regDiv.querySelector('.reg-name').value.trim();const regType=regDiv.querySelector('.reg-type').value;const regAddr=parseInt(regDiv.querySelector('.reg-addr').value,10);if(regName&&!isNaN(regAddr)){return{name:regName,type:regType,address:regAddr};}return null;}).filter(x=>x);if(!isNaN(id)&&name){return{id,name,poll:!isNaN(poll)?poll:1000,registers};}return null;}).filter(x=>x);}
function renderDynamicLists(){if(currentIoa.modbus_slaves&&Array.isArray(currentIoa.modbus_slaves)){currentIoa.modbus_slaves.forEach(o=>addModbusSlaveRowNew(o));}if(currentIoa.modbus_list&&Array.isArray(currentIoa.modbus_list)){currentIoa.modbus_list.forEach(o=>addListRow('modbus',o));}}
function toggleModbusVisibility(){const enabled=byId('feat_modbus')?.checked;const core=byId('modbus-core');const slaves=byId('modbus-slaves-section');const regs=byId('modbus-registers-section');if(core)core.style.display=enabled?'grid':'none';if(slaves)slaves.style.display=enabled?'block':'none';if(regs)regs.style.display=enabled?'block':'none';}
function applyAutoDisableIOA(){document.querySelectorAll('[data-ioa]').forEach(inp=>{const inputName=inp.name;let isFeatureEnabled=false;const featureMapping={'remote_cb_1':'feat_cb1','status_cb_1':'feat_cb1','command_cb_1':'feat_cb1','remote_cb_2':'feat_cb2','status_cb_2':'feat_cb2','command_cb_2':'feat_cb2','remote_cb_3':'feat_cb3','status_cb_3':'feat_cb3','command_cb_3':'feat_cb3','di_gfd':'feat_gfd','di_supply_status':'feat_supply','di_door':'feat_door','ai_temperature':'feat_temp','ai_humidity':'feat_humidity','di1':'feat_di1','di2':'feat_di2','di3':'feat_di3','di4':'feat_di4','di5':'feat_di5','do1':'feat_do1','do2':'feat_do2'};const mappedFeature=featureMapping[inputName];if(mappedFeature){const toggle=byId(mappedFeature);isFeatureEnabled=toggle&&toggle.checked;}else{const featName=inputName.replace(/^(di_|ai_|do_|status_|command_|remote_)/,'');const toggles=document.querySelectorAll('input[type="checkbox"]');toggles.forEach(t=>{if(t.checked&&(t.id.includes(featName)||featName.includes(t.id.replace('feat_','')))){isFeatureEnabled=true;}});}inp.disabled=!isFeatureEnabled;if(!isFeatureEnabled)inp.value='';});}
function showNotif(msg,isError){const n=byId('notif');if(!n)return;if(n.hideTimeout){clearTimeout(n.hideTimeout);delete n.hideTimeout;}n.textContent=msg;let type='info';if(isError===true)type='error';else if(isError===false)type='success';else if(typeof isError==='string')type=isError;n.className='notif-bar '+type;n.style.display='block';const hideDelay=(type==='error')?8000:5000;n.hideTimeout=setTimeout(()=>{n.style.display='none';delete n.hideTimeout;},hideDelay);} 
document.addEventListener('DOMContentLoaded',()=>{const addSlaveBtn=byId('add-modbus-slave');if(addSlaveBtn)addSlaveBtn.addEventListener('click',addModbusSlaveRowNew);const formEl=byId('device-form');if(formEl)formEl.addEventListener('submit',e=>{e.preventDefault();onUnifiedSubmit(e);});const restartBtn=byId('btnRestart');if(restartBtn)restartBtn.addEventListener('click',()=>{if(!confirm('Yakin restart RTU sekarang?'))return;showNotif('Restarting...');fetch('/api/restart',{method:'POST'}).then(r=>r.text()).then(t=>{showNotif(t||'Restart issued');}).catch(()=>{showNotif('Gagal kirim restart',true);});});['feat_gfd','feat_supply','feat_door','feat_temp','feat_humidity','feat_cb1','feat_cb2','feat_cb3','feat_modbus','feat_di1','feat_di2','feat_di3','feat_di4','feat_di5','feat_do1','feat_do2'].forEach(id=>{const el=byId(id);if(el)el.addEventListener('change',()=>{renderGroupIoa();toggleModbusVisibility();applyAutoDisableIOA();});});});
function filterLogBox(){const box=byId('logBox');if(!box||!box.dataset.raw)return;const wasAtBottom=box.scrollTop>=(box.scrollHeight-box.clientHeight-5);const rawLines=box.dataset.raw.split('\n');const activeFilters=[];document.querySelectorAll('.logCat:checked').forEach(cb=>{const category=cb.id.replace('log_cat_','').toUpperCase();activeFilters.push(category);});const categoryPatterns={'SYSTEM':['[SYSTEM]','[SYS]','BOOT','RESET','RESTART','STARTUP'],'IEC104':['[IEC104]','[IEC]','ASDU','APDU','TESTFR','STARTDT','STOPDT'],'MODBUS':['[MODBUS]','[MB]','RTU','TCP','SLAVE','REGISTER'],'SOE':['[SOE]','EVENT','SEQUENCE'],'COMM':['[COMM]','[COMMUNICATION]','CONNECT','DISCONNECT','SEND','RECV'],'ETHERNET':['[ETH]','[ETHERNET]','MAC','IP','DHCP','LINK'],'MODEM':['[MODEM]','[GSM]','[LTE]','SIM800','SIM7600','QUECTEL','APN','SIGNAL'],'HAL':['[HAL]','[HARDWARE]','GPIO','ADC','PWM','I2C','SPI','UART'],'WEB':['[WEB]','[HTTP]','[SERVER]','GET','POST','WEBSOCKET'],'CONFIG':['[CONFIG]','[CFG]','SAVE','LOAD','NVRAM','EEPROM'],'TASK':['[TASK]','[THREAD]','STACK','HEAP','MEMORY'],'TIME':['[TIME]','[RTC]','[NTP]','SYNC','TIMESTAMP'],'ERROR':['[ERROR]','[ERR]','FAIL','TIMEOUT','EXCEPTION','CRITICAL'],'DEBUG':['[DEBUG]','[DBG]','TRACE','VERBOSE'],'WIFI':['[WIFI]','[WLAN]','SSID','PASSWORD','CONNECTED','SCAN'],'NTP':['[NTP]','TIME_SYNC','SNTP','TIMESERVER']};let filteredLines=rawLines;if(activeFilters.length>0){filteredLines=rawLines.filter(line=>activeFilters.some(category=>{const patterns=categoryPatterns[category]||[];return patterns.some(pattern=>line.toUpperCase().includes(pattern));}));}box.textContent=filteredLines.join('\n');const autoScroll=byId('autoScrollLogs');if(autoScroll?.checked&&wasAtBottom){setTimeout(()=>{box.scrollTop=box.scrollHeight;},10);}const totalLines=rawLines.length;const filteredCount=filteredLines.length;let statusEl=byId('logFilterStatus');if(!statusEl){statusEl=document.createElement('div');statusEl.id='logFilterStatus';statusEl.style.cssText='font-size:10px;color:#666;margin-top:4px';box.parentNode.appendChild(statusEl);}statusEl.textContent=`Showing ${filteredCount}/${totalLines} lines (${activeFilters.length} filters active)`;}
document.addEventListener('DOMContentLoaded',()=>{const applyBtn=byId('btnApplyLogFilter');if(applyBtn)applyBtn.addEventListener('click',filterLogBox);document.querySelectorAll('.logCat').forEach(cb=>{cb.addEventListener('change',()=>{if(byId('autoLogs')?.checked){filterLogBox();}});});const firstCat=document.querySelector('.logCat');if(firstCat){const filterContainer=firstCat.closest('div');if(filterContainer){const selectAllBtn=document.createElement('button');selectAllBtn.textContent='All';selectAllBtn.type='button';selectAllBtn.style.cssText='font-size:10px;padding:2px 6px;margin-left:8px';selectAllBtn.addEventListener('click',()=>{document.querySelectorAll('.logCat').forEach(cb=>cb.checked=true);filterLogBox();});const selectNoneBtn=document.createElement('button');selectNoneBtn.textContent='None';selectNoneBtn.type='button';selectNoneBtn.style.cssText='font-size:10px;padding:2px 6px;margin-left:4px';selectNoneBtn.addEventListener('click',()=>{document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);filterLogBox();});filterContainer.appendChild(selectAllBtn);filterContainer.appendChild(selectNoneBtn);}}const presetButtons={'btnFilterComm':['COMM','ETHERNET','MODEM','WIFI'],'btnFilterErrors':['ERROR'],'btnFilterProtocol':['IEC104','MODBUS'],'btnFilterSystem':['SYSTEM','HAL','TASK','CONFIG']};Object.entries(presetButtons).forEach(([btnId,categories])=>{const btn=byId(btnId);if(btn){btn.addEventListener('click',()=>{document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);categories.forEach(cat=>{const checkbox=byId(`log_cat_${cat.toLowerCase()}`);if(checkbox)checkbox.checked=true;});filterLogBox();});}});});
</script>
</script>
</body></html>
)HTML";

// ============================= MONITOR PAGE ===============================
const char MONITOR_PAGE[] PROGMEM = R"HTML(
<!DOCTYPE html><html lang='en'>
<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>
<title>GOES Configurator - Monitoring</title>
<style>
 :root { --accent:#0078d4; --accent-hover:#0062ad; --bg:#f2f5f9; --panel:#fff; --border:#d4dbe4; }
 body{font-family:Arial,Helvetica,sans-serif;margin:0;background:var(--bg);color:#222}
 header{background:#1a3d6d;color:#fff;padding:12px 22px;position:sticky;top:0;z-index:1000;display:flex;align-items:center;justify-content:space-between;box-shadow:0 2px 6px rgba(0,0,0,.25)}
 .notif-bar{position:fixed;top:70px;left:50%;transform:translateX(-50%);z-index:1100;background:#fff4c2;color:#664d00;font-size:14px;font-weight:600;padding:12px 24px;display:none;box-shadow:0 4px 12px rgba(0,0,0,.25);border-radius:8px;border:1px solid #e6d700;max-width:90%;text-align:center;animation:slideInDown 0.3s ease-out}
 @keyframes slideInDown{from{opacity:0;transform:translate(-50%,-20px)}to{opacity:1;transform:translate(-50%,0)}}
 h1{margin:0;font-size:19px;letter-spacing:.5px;font-weight:600}
 main{padding:18px;max-width:1220px;margin:0 auto}
 h2{margin:28px 0 12px;font-size:17px;color:#1a3d6d}
 fieldset{border:1px solid var(--border);margin:0 0 18px;padding:14px 16px;border-radius:10px;background:var(--panel)}
 legend{padding:0 6px;font-weight:600;font-size:13px;color:#1a3d6d}
 label.lbl{display:block;margin:0 0 4px;font-weight:600;font-size:11px;letter-spacing:.3px;text-transform:uppercase;color:#425b76}
 input[type=text],input[type=number],select{width:100%;padding:8px 10px;border:1px solid #b8c4d1;border-radius:6px;font-size:14px;background:#fff;box-sizing:border-box}
 button{background:var(--accent);color:#fff;padding:10px 20px;border:none;border-radius:6px;font-size:14px;font-weight:600;cursor:pointer;box-shadow:0 2px 4px rgba(0,0,0,.15);transition:.18s}
 button:hover{background:var(--accent-hover)}
 .grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(210px,1fr));gap:14px}
 .two-col{display:grid;grid-template-columns:repeat(auto-fit,minmax(340px,1fr));gap:18px}
 .muted{font-size:11px;color:#667}
 .status{display:none}
 #liveStatusPanel span { color: #333; }
 #liveStatusPanel strong, #liveStatusPanel code { color: #000; font-weight: 600; }
 #liveStatusPanel.offline{background:#fdeeee;border-color:#f2b8b5}
 #liveStatusPanel.prototype{background:#fff3cd;border:1px solid #ffeaa7;color:#856404}
 .prototype-badge{background:#f39c12;color:#fff;font-size:10px;padding:3px 8px;border-radius:12px;font-weight:700;letter-spacing:.5px;text-transform:uppercase}
 footer{padding:14px 24px;text-align:center;font-size:11px;color:#678;background:#f0f4f8;margin-top:40px;border-top:1px solid #d5dce3}
 pre#logBox{background:#111;color:#0f0;padding:10px;height:260px;overflow:auto;font-size:11px;border-radius:6px;border:1px solid #333}
 .toggles{display:grid;grid-template-columns:repeat(auto-fill,minmax(170px,1fr));gap:10px}
 .toggle-card{display:flex;align-items:center;justify-content:space-between;padding:8px 10px;background:#ffffff;border:1px solid #e1e6ec;border-radius:8px;font-size:11px;font-weight:600;box-shadow:0 1px 2px rgba(0,0,0,.06)}
 .switch{position:relative;display:inline-block;width:42px;height:22px}
 .switch input{opacity:0;width:0;height:0}
 .slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#b5bcc5;transition:.3s;border-radius:34px}
 .slider:before{position:absolute;content:"";height:18px;width:18px;left:2px;top:2px;background:#fff;transition:.3s;border-radius:50%;box-shadow:0 1px 3px rgba(0,0,0,.35)}
 .switch input:checked + .slider{background:var(--accent)}
 .switch input:checked + .slider:before{transform:translateX(20px)}
 .group-label{grid-column:1/-1;margin-top:8px;font-size:9.5px;font-weight:700;letter-spacing:.7px;color:#555;text-transform:uppercase}
 .save-bar{padding:10px 0;margin-top:18px;display:flex;gap:12px;align-items:center}
 .tag{background:#1a3d6d;color:#fff;font-size:10px;padding:3px 7px;border-radius:11px;margin-left:10px;letter-spacing:.5px}
 .notif-bar.error{background:#ff4757;color:#fff;border-color:#ff3838}
 .notif-bar.info{background:#2ed573;color:#fff;border-color:#20bf6b}
 .notif-bar.success{background:#2ed573;color:#fff;border-color:#20bf6b}
 .notif-bar.warning{background:#ffa502;color:#fff;border-color:#ff9500}
 section{background:var(--panel);border-radius:10px;padding:18px;margin-bottom:18px;box-shadow:0 2px 8px rgba(0,0,0,0.1);border:1px solid var(--border)}
.switch{position:relative;display:inline-block;width:40px;height:20px}
.switch input{opacity:0;width:0;height:0}
.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background:#ccc;transition:.4s;border-radius:20px}
.slider:before{position:absolute;content:"";height:16px;width:16px;left:2px;bottom:2px;background:white;transition:.4s;border-radius:50%}
input:checked+.slider{background:#4CAF50}
input:checked+.slider:before{transform:translateX(20px)}
button{background:#1a3d6d;color:#fff;border:none;padding:8px 16px;border-radius:4px;cursor:pointer;font-size:12px;margin:2px}
button:hover{background:#2c5282}
button:disabled{background:#ccc;cursor:not-allowed}
input,select,textarea{padding:6px;border:1px solid #ccc;border-radius:4px;font-size:12px}
.muted{color:#666;font-size:11px}
.status{margin:10px 0;padding:8px;border-radius:4px;display:none}
.status.success{background:#d4edda;color:#155724;border:1px solid #c3e6cb}
.status.error{background:#f8d7da;color:#721c24;border:1px solid #f5c6cb}
#logBox{background:#000;color:#0f0;padding:10px;border-radius:4px;max-height:400px;overflow:auto;font-family:monospace;font-size:11px;white-space:pre-wrap;word-wrap:break-word}
#statusGrid{display:grid;grid-template-columns:repeat(auto-fit,minmax(200px,1fr));gap:15px;margin-top:15px}
.status-card{background:#f8f9fa;border:1px solid #dee2e6;border-radius:6px;padding:12px}
.status-card h4{margin:0 0 8px 0;color:#1a3d6d;font-size:14px}
.status-value{font-weight:bold;font-size:16px;margin:4px 0}
.status-value.on{color:#28a745}
.status-value.off{color:#dc3545}
.status-value.fault{color:#ffc107}
footer{text-align:center;padding:20px;color:#666;font-size:12px}
</style>
</head><body>
<header><h1>GOES RTU Monitoring</h1><span class='tag'>v2.x-proto</span>
<nav style='margin-left:auto'>
	<a href='/' style='color:#ccc;text-decoration:none;margin-right:15px;font-weight:600'>Configuration</a>
	<a href='/monitor' style='color:#fff;text-decoration:none;font-weight:600'>Monitoring</a>
</nav>
</header>
<div id='liveStatusPanel' style='display:flex;flex-wrap:wrap;align-items:center;gap:10px 18px;padding:8px 22px;background:#e9f2ff;border-bottom:1px solid #c4d9f2;font-size:11.5px'>
 <span class='prototype-badge'>Prototype Mode</span>
 <span id='ls_conn'>Polling...</span>
 <span>| Comm: <strong id='ls_comm'>-</strong></span>
 <span>| IEC104 Tx:<span id='ls_iec104_tx'>-</span></span>
 <span>| IEC104 Rx:<span id='ls_iec104_rx'>-</span></span>
 <span>| IEC104 Last(ms):<span id='ls_iec104_last'>-</span></span>
 <span>| Uptime: <span id='ls_uptime'>-</span></span>
 <span>| Heap: <span id='ls_heap'>-</span></span>
 <span>| Stack(main): <span id='ls_stack'>-</span></span>
 <span>| Stack(comm): <span id='ls_stack_comm'>-</span></span>
 <span>| Stack(iec104): <span id='ls_stack_iec104'>-</span></span>
 <span>| Stack(hal): <span id='ls_stack_hal'>-</span></span>
 <span>| Rev: <code id='ls_rev'>-</code></span>
</div>
 <!-- Prominent RTU Clock Bar -->
 <div id='rtuClockBar' style='display:flex;align-items:center;gap:14px;padding:10px 22px;background:#102a46;color:#fff;font-size:14px;font-weight:600;border-bottom:2px solid #0d2238'>
	 <div style='font-size:12px;letter-spacing:.5px;text-transform:uppercase;opacity:.85'>RTU Clock</div>
	 <div style='font-family:monospace;font-size:16px' id='rtu_time_full'>--</div>
	 <div style='font-size:11px;background:#1f4d80;padding:3px 8px;border-radius:12px' id='rtu_time_source'>Source: -</div>
	 <div style='margin-left:auto;font-size:11px;opacity:.8'>Updated <span id='rtu_time_updated'>-</span></div>
 </div>
<div id='notif' class='notif-bar'></div><main>

<section>
 <h2>System Status</h2>
 <div style='display:flex;gap:15px;align-items:center;margin-bottom:15px'>
	 <button type='button' id='btnRefreshStatus'>Refresh Status</button>
	 <button type='button' id='btnRefreshConfig'>Reload Config</button>
	 <div style='display:flex;align-items:center;gap:8px;background:#f8f9fa;padding:8px 12px;border-radius:8px;border:1px solid #dee2e6'>
		 <span style='font-size:12px;font-weight:600'>Auto Refresh:</span>
		 <label class='switch'><input type='checkbox' id='autoStatus' checked><span class='slider'></span></label>
		 <span style='font-size:11px;color:#666'>(5s)</span>
	 </div>
	 <div style='font-size:11px;color:#666;margin-left:auto'>
		 Config loaded: <span id='configStatus'>Unknown</span>
	 </div>
 </div>
 <div id='statusGrid'></div>
</section>

 <!-- Main Features Section -->
 <section id='main-features-section'>
	 <h2>Main Features (IEC104)</h2>
	 <div style='overflow:auto'>
		 <table style='width:100%;border-collapse:collapse;font-size:12px'>
			 <thead style='background:#1a3d6d;color:#fff;position:sticky;top:0'>
				 <tr>
					 <th style='padding:6px 8px;text-align:left;border:1px solid #ccc'>Feature</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>IOA</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Data Type</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Value</th>
				 </tr>
			 </thead>
			 <tbody id='mainFeaturesBody'></tbody>
		 </table>
	 </div>
	 <div class='muted' id='mainFeaturesEmpty' style='display:none;margin-top:6px'>No main features enabled in configuration.</div>
 </section>

 <!-- Extended Features Section -->
 <section id='extended-features-section'>
	 <h2>Extended Features (IEC104)</h2>
	 <div style='overflow:auto'>
		 <table style='width:100%;border-collapse:collapse;font-size:12px'>
			 <thead style='background:#1a3d6d;color:#fff;position:sticky;top:0'>
				 <tr>
					 <th style='padding:6px 8px;text-align:left;border:1px solid #ccc'>Feature</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>IOA</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Data Type</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Value</th>
				 </tr>
			 </thead>
			 <tbody id='extFeaturesBody'></tbody>
		 </table>
	 </div>
	 <div class='muted' id='extFeaturesEmpty' style='display:none;margin-top:6px'>No extended features enabled in configuration.</div>
 </section>

 <!-- Modbus Values Section -->
 <section id='modbus-values-section'>
	 <h2>Modbus Register Values</h2>
	 <div style='overflow:auto;max-height:400px'>
		 <table style='width:100%;border-collapse:collapse;font-size:12px'>
			 <thead style='background:#1a3d6d;color:#fff;position:sticky;top:0'>
				 <tr>
					 <th style='padding:6px 8px;text-align:left;border:1px solid #ccc'>Slave</th>
					 <th style='padding:6px 8px;text-align:left;border:1px solid #ccc'>Register</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Address</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Data Type</th>
					 <th style='padding:6px 8px;text-align:center;border:1px solid #ccc'>Value</th>
				 </tr>
			 </thead>
			 <tbody id='modbusValuesBody'></tbody>
		 </table>
	 </div>
	 <div class='muted' id='modbusValuesEmpty' style='display:none;margin-top:6px'>No Modbus registers configured.</div>
 </section>

<section id='soe-section'>
 <h2>SOE (Sequence of Events)</h2>
 <div style='display:flex;gap:10px;align-items:center;margin-bottom:8px'>
		<button type='button' id='btnRefreshSoe'>Refresh SOE</button>
		<button type='button' id='btnClearSoe'>Clear SOE</button>
		<label style='font-size:12px'><input type='checkbox' id='autoSoe' checked> Auto (3s)</label>
		<span id='soeSummary' class='muted'>-</span>
 </div>
 <div style='max-height:240px;overflow:auto;border:1px solid #ccc;background:#fff;border-radius:6px'>
	 <table style='width:100%;border-collapse:collapse;font-size:11.5px'>
			<thead style='position:sticky;top:0;background:#1a3d6d;color:#fff'>
				 <tr><th style='text-align:left;padding:4px 6px'>Seq</th><th style='text-align:left;padding:4px 6px'>Millis</th><th style='text-align:left;padding:4px 6px'>IOA</th><th style='text-align:left;padding:4px 6px'>Val</th></tr>
			</thead>
			<tbody id='soeBody'></tbody>
	 </table>
 </div>
</section>

<section style='padding:12px' id='logs-section'>
 <h2>Logs</h2>
 <div style='display:flex;flex-wrap:wrap;gap:12px;align-items:center;margin-bottom:8px'>
	<button type='button' id='btnRefreshLogs'>Refresh</button>
  
	<div style='display:flex;align-items:center;gap:8px;background:#f8f9fa;padding:8px 12px;border-radius:8px;border:1px solid #dee2e6'>
		<span style='font-size:12px;font-weight:600'>Enable Logs:</span>
		<label class='switch'><input type='checkbox' id='log_enable' checked><span class='slider'></span></label>
	</div>
  
	<div style='display:flex;align-items:center;gap:8px;background:#f8f9fa;padding:8px 12px;border-radius:8px;border:1px solid #dee2e6'>
		<span style='font-size:12px;font-weight:600'>Auto Refresh:</span>
		<label class='switch'><input type='checkbox' id='autoLogs' checked><span class='slider'></span></label>
		<span style='font-size:11px;color:#666'>(2s)</span>
	</div>
  
	<div style='display:flex;align-items:center;gap:8px;background:#f8f9fa;padding:8px 12px;border-radius:8px;border:1px solid #dee2e6'>
		<span style='font-size:12px;font-weight:600'>Auto Scroll:</span>
		<label class='switch'><input type='checkbox' id='autoScrollLogs' checked><span class='slider'></span></label>
	</div>
	<div style='display:flex;flex-wrap:wrap;gap:6px;font-size:11px;background:#fff;padding:6px 8px;border:1px solid #ccc;border-radius:6px'>
		<label><input type='checkbox' class='logCat' id='log_cat_system' checked> System</label>
		<label><input type='checkbox' class='logCat' id='log_cat_iec104' checked> IEC104</label>
		<label><input type='checkbox' class='logCat' id='log_cat_modbus' checked> Modbus</label>
		<label><input type='checkbox' class='logCat' id='log_cat_soe' checked> SOE</label>
		<label><input type='checkbox' class='logCat' id='log_cat_comm' checked> Comm</label>
		<label><input type='checkbox' class='logCat' id='log_cat_ethernet' checked> Ethernet</label>
		<label><input type='checkbox' class='logCat' id='log_cat_modem' checked> Modem</label>
		<label><input type='checkbox' class='logCat' id='log_cat_hal' checked> HAL</label>
		<label><input type='checkbox' class='logCat' id='log_cat_web' checked> Web</label>
		<label><input type='checkbox' class='logCat' id='log_cat_config' checked> Config</label>
		<label><input type='checkbox' class='logCat' id='log_cat_task' checked> Task</label>
		<label><input type='checkbox' class='logCat' id='log_cat_time' checked> Time</label>
		<label><input type='checkbox' class='logCat' id='log_cat_error' checked> Error</label>
		<label><input type='checkbox' class='logCat' id='log_cat_debug' checked> Debug</label>
		<label><input type='checkbox' class='logCat' id='log_cat_wifi' checked> WiFi</label>
		<label><input type='checkbox' class='logCat' id='log_cat_ntp' checked> NTP</label>
	</div>
	<input type='number' id='logCap' placeholder='Lines' style='width:80px'>
	<button type='button' id='btnSetLogCap'>Set Cap</button>
	<button type='button' id='btnClearLogs'>Clear Logs</button>
	<button type='button' id='btnApplyLogFilter'>Apply Filters</button>
	<div style='margin-left:12px;display:flex;gap:4px;align-items:center'>
		<label style='font-size:10px;color:#666'>Presets:</label>
		<button type='button' id='btnFilterComm' style='font-size:10px;padding:2px 6px'>Comm Only</button>
		<button type='button' id='btnFilterErrors' style='font-size:10px;padding:2px 6px'>Errors Only</button>
		<button type='button' id='btnFilterProtocol' style='font-size:10px;padding:2px 6px'>Protocols</button>
		<button type='button' id='btnFilterSystem' style='font-size:10px;padding:2px 6px'>System</button>
	</div>
 </div>
 <pre id='logBox'></pre>
 <div class='muted' style='margin-top:4px'>Filter berbasis prefix baris misal [IEC104],[MODBUS],[SOE]. Backend belum kirim kategorisasi eksplisit.</div>
</section>

</main><footer>GOES &copy; 2025</footer>

<script>
// Mock API System for Prototype (monitor page)
class MockAPI{constructor(){this.logs=this.generateMockLogs();this.soe=this.generateMockSOE();this.loadSavedState();this.status=this.generateMockStatus();}generateMockLogs(){const categories=['SYSTEM','IEC104','MODBUS','SOE','COMM','ETHERNET','MODEM','HAL','WEB','CONFIG','TASK','TIME','ERROR','DEBUG','WIFI','NTP'];const logs=[];const now=Date.now();for(let i=0;i<50;i++){const timestamp=new Date(now-(50-i)*5000).toISOString().substring(11,23);const category=categories[Math.floor(Math.random()*categories.length)];const messages={'SYSTEM':['System initialized','Watchdog reset','Boot complete','Memory check OK'],'IEC104':['Client connected','Frame received','General interrogation','Connection timeout'],'MODBUS':['TCP server started','Register read','Exception response','Slave response'],'SOE':['Event logged','Buffer full','Timestamp sync','Sequence increment'],'COMM':['Interface up','Data received','Transmission error','Link status'],'ERROR':['Configuration error','Memory allocation failed','Timeout occurred','Invalid parameter']};const message=messages[category]?messages[category][Math.floor(Math.random()*messages[category].length)]:'Generic message';logs.push(`[${timestamp}] [${category}] ${message}`);}return logs;}generateMockSOE(){const soe=[];const now=Date.now();for(let i=0;i<20;i++){soe.push({seq:i+1,millis:now-(20-i)*10000,ioa:1000+Math.floor(Math.random()*100),val:Math.random()>0.5?1:0});}return soe;}generateMockStatus(){const commMethod=this.config?.comm_method||'wifi';const now=new Date();const fullTs=now.getFullYear()+ '-' + String(now.getMonth()+1).padStart(2,'0') + '-' + String(now.getDate()).padStart(2,'0')+ ' ' + String(now.getHours()).padStart(2,'0') + ':' + String(now.getMinutes()).padStart(2,'0') + ':' + String(now.getSeconds()).padStart(2,'0') + '.' + String(now.getMilliseconds()).padStart(3,'0');const src=(this.config?.time_source||'RTC');return{system:{uptime:'2d 5h 30m',memory_free:'45.2 KB',cpu_usage:'12%',temperature:'42°C',rtu_time:fullTs,rtu_time_source:src},esp32:{chip_model:'ESP32-D0WD-V3',chip_revision:'3.0',flash_size:'4MB',core_count:'2',cpu_freq:240,heap_free:Math.floor(Math.random()*50000+200000)+' bytes',stack_free:Math.floor(Math.random()*5000+15000)+' bytes',reset_reason:'Power-on Reset'},communication:this.generateCommStatus(commMethod),protocols:{iec104_status:Math.random()>0.3?'Connected':'LRU Fail',iec104_clients:Math.random()>0.3?Math.floor(Math.random()*3)+1:0,iec104_last_contact:Math.random()>0.3?Math.floor(Math.random()*30)+'s ago':'Never',iec104_common_addr:this.config?.iec_common_addr||'1',iec104_k:this.config?.iec_k||'12',iec104_w:this.config?.iec_w||'8',iec104_frames_sent:Math.floor(Math.random()*10000),iec104_frames_received:Math.floor(Math.random()*8000),modbus_status:'Master Active',modbus_baud:this.config?.modbus_baud||'9600',modbus_parity:this.config?.modbus_parity||'None',modbus_poll_ms:this.config?.modbus_poll_ms||'1000',modbus_polls_success:Math.floor(Math.random()*15)+85,modbus_last_poll:Math.floor(Math.random()*5)+1+'s ago',modbus_slaves:this.generateModbusSlaveStatus()},main_features:this.generateMainFeatures(),extended_features:this.generateExtendedFeatures(),modbus_values:this.generateModbusValues()};}updateStatus(){this.status=this.generateMockStatus();try{displayStatus(this.status);updateLiveStatusPanel();updateRtuClockBar();renderFeatureTables(this.status);}catch(e){}}generateMainFeatures(){const features={};if(this.config?.feat_gfd)features.gfd=Math.random()>0.2?1:0;if(this.config?.feat_supply)features.supply=Math.floor(Math.random()*4);if(this.config?.feat_cb1)features.cb_1=Math.floor(Math.random()*4);if(this.config?.feat_cb2)features.cb_2=Math.floor(Math.random()*4);if(this.config?.feat_cb3)features.cb_3=Math.floor(Math.random()*4);return features;}generateExtendedFeatures(){const features={};if(this.config?.feat_door)features.door=Math.random()>0.5?1:0;if(this.config?.feat_temp)features.temperature_sensor=(Math.random()*30+20).toFixed(1);if(this.config?.feat_humidity)features.humidity_sensor=(Math.random()*40+30).toFixed(1);if(this.config?.feat_di1)features.di_1=Math.random()>0.5?1:0;if(this.config?.feat_di2)features.di_2=Math.random()>0.5?1:0;if(this.config?.feat_di3)features.di_3=Math.random()>0.5?1:0;if(this.config?.feat_di4)features.di_4=Math.random()>0.5?1:0;if(this.config?.feat_di5)features.di_5=Math.random()>0.5?1:0;return features;}generateModbusValues(){const values={};let allSlaves=[];if(this.ioaConfig?.modbus_slaves)allSlaves=allSlaves.concat(this.ioaConfig.modbus_slaves);if(this.modbusConfig?.slaves)allSlaves=allSlaves.concat(this.modbusConfig.slaves);allSlaves.forEach(slave=>{if(slave.registers&&slave.registers.length>0){slave.registers.forEach(register=>{const regKey=`${slave.name}_${register.name}`;switch(register.type){case 'holding':case 'input':values[regKey]=Math.floor(Math.random()*65536);break;case 'coil':case 'discrete':values[regKey]=Math.random()>0.5?1:0;break;default:values[regKey]=Math.floor(Math.random()*65536);}});}});return values;}generateCommStatus(method){switch(method){case 'wifi':return{type:'WiFi',status:Math.random()>0.2?'Connected':'Disconnected',ssid:this.config?.wifi_ssid||'GOES_Network',rssi:Math.floor(Math.random()*40)-80+' dBm',ip_address:'192.168.1.'+(Math.floor(Math.random()*200)+50),gateway:'192.168.1.1',netmask:'255.255.255.0',dns:'8.8.8.8',channel:Math.floor(Math.random()*11)+1,encryption:'WPA2-PSK'};case 'ethernet':return{type:'Ethernet',status:Math.random()>0.1?'Link Up':'Link Down',mac_address:this.config?.eth_mac||'00:11:22:33:44:55',speed:'100 Mbps',duplex:'Full',ip_address:this.config?.eth_ip||'192.168.1.'+(Math.floor(Math.random()*200)+50),gateway:this.config?.eth_gateway||'192.168.1.1',netmask:this.config?.eth_netmask||'255.255.255.0',dns:'8.8.8.8',link_status:Math.random()>0.1?'Up':'Down',auto_negotiation:'Enabled'};case 'modem':const modemType=this.config?.modem_type||'sim7600ce';const signalStrength=Math.floor(Math.random()*31);return{type:modemType.toUpperCase(),status:Math.random()>0.25?'Connected':'No Signal',operator:Math.random()>0.5?'Telkomsel':'XL Axiata',signal_strength:signalStrength,signal_quality:signalStrength>20?'Excellent':signalStrength>15?'Good':signalStrength>10?'Fair':'Poor',network_type:Math.random()>0.5?'4G LTE':'3G HSPA',ip_address:'10.'+Math.floor(Math.random()*255)+'.'+Math.floor(Math.random()*255)+'.'+Math.floor(Math.random()*255),local_ip:'192.168.43.1',apn:this.config?.apn||'internet',imei:'86'+Math.floor(Math.random()*1000000000000000),sim_status:Math.random()>0.1?'Ready':'Not Inserted',registration:Math.random()>0.2?'Registered':'Searching'};default:return{type:'Unknown',status:'Disconnected',ip_address:'N/A'};}}generateModbusSlaveStatus(){if(this.ioaConfig?.modbus_slaves&&this.ioaConfig.modbus_slaves.length>0){return this.ioaConfig.modbus_slaves.map(slave=>{const slaveStatus=this.generateSlaveStatus();return{id:slave.id,name:slave.name||`Slave ${slave.id}`,address:slave.address||0,poll_interval:slave.poll||this.config?.modbus_poll_ms||1000,status:slaveStatus,last_poll:Math.floor(Math.random()*10)+1+'s ago',error_count:Math.floor(Math.random()*5),success_rate:Math.floor(Math.random()*15)+85+'%',registers:slave.registers?slave.registers.map(reg=>({name:reg.name||`Register ${reg.address}`,address:reg.address,value:slaveStatus==='Online'?Math.floor(Math.random()*1000)+100:'N/A'})):[]};});}return[{id:1,name:'Temperature Sensor',status:this.generateSlaveStatus(),address:40001,poll_interval:1000,last_poll:Math.floor(Math.random()*10)+1+'s ago',error_count:Math.floor(Math.random()*3),success_rate:Math.floor(Math.random()*15)+85+'%',registers:[{name:'Temperature',address:40001,value:(Math.random()*50+20).toFixed(1)+'°C'},{name:'Humidity',address:40002,value:(Math.random()*40+40).toFixed(1)+'%'}]},{id:2,name:'Pressure Meter',status:this.generateSlaveStatus(),address:30001,poll_interval:2000,last_poll:Math.floor(Math.random()*15)+1+'s ago',error_count:Math.floor(Math.random()*2),success_rate:Math.floor(Math.random()*10)+90+'%',registers:[{name:'Pressure',address:30001,value:(Math.random()*1000+100).toFixed(0)},{name:'Flow Rate',address:30002,value:(Math.random()*5000+500).toFixed(0)}]},{id:247,name:'Remote Terminal',status:this.generateSlaveStatus(),address:10001,poll_interval:5000,last_poll:Math.floor(Math.random()*30)+1+'s ago',error_count:Math.floor(Math.random()*5),success_rate:Math.floor(Math.random()*20)+80+'%',registers:[{name:'Status Word',address:10001,value:Math.floor(Math.random()*65536)},{name:'Control Word',address:10002,value:Math.floor(Math.random()*65536)},{name:'Alarm Count',address:10003,value:Math.floor(Math.random()*10)}]}];}generateSlaveStatus(){const statuses=['Online','Offline','Timeout','Error'];const weights=[0.7,0.15,0.1,0.05];const random=Math.random();let cumulative=0;for(let i=0;i<statuses.length;i++){cumulative+=weights[i];if(random<=cumulative){return statuses[i];}}return'Online';}loadSavedState(){const saved=localStorage.getItem('goesMonitorState');if(saved){const state=JSON.parse(saved);if(state.autoStatus!==undefined)document.getElementById('autoStatus').checked=state.autoStatus;if(state.autoSoe!==undefined)document.getElementById('autoSoe').checked=state.autoSoe;if(state.autoLogs!==undefined)document.getElementById('autoLogs').checked=state.autoLogs;if(state.autoScrollLogs!==undefined)document.getElementById('autoScrollLogs').checked=state.autoScrollLogs;if(state.logEnable!==undefined)document.getElementById('log_enable').checked=state.logEnable;}this.loadConfigState();}loadConfigState(){const configState=localStorage.getItem('goesConfigState');if(configState){const newConfig=JSON.parse(configState);const configChanged=JSON.stringify(this.config)!==JSON.stringify(newConfig);this.config=newConfig;const statusEl=document.getElementById('configStatus');if(statusEl){statusEl.textContent=new Date().toLocaleTimeString();statusEl.style.color='#28a745';}if(configChanged){this.triggerConfigUpdate();}}else{this.config={comm_method:'wifi',modem_type:'sim7600ce',wifi_ssid:'GOES_Network',feat_gfd:true,feat_supply:true,feat_cb1:true,feat_cb2:true,feat_cb3:false,feat_door:true,feat_temp:true,feat_humidity:true,feat_di1:true,feat_di2:false,feat_di3:false,feat_di4:false,feat_di5:false,feat_do1:true,feat_do2:false};const statusEl=document.getElementById('configStatus');if(statusEl){statusEl.textContent='Default';statusEl.style.color='#ffc107';}}this.loadIOAConfig();}triggerConfigUpdate(){clearInterval(this.updateInterval);if(typeof this.updateStatus==='function'){this.updateStatus();this.updateInterval=setInterval(()=>this.updateStatus(),5000);}showNotification('Configuration updated, monitoring refreshed','info');}loadIOAConfig(){const ioaState=localStorage.getItem('prototype_ioa_config');if(ioaState){const newIOAConfig=JSON.parse(ioaState);const ioaConfigChanged=JSON.stringify(this.ioaConfig)!==JSON.stringify(newIOAConfig);this.ioaConfig=newIOAConfig;if(ioaConfigChanged&&this.ioaConfig){this.triggerConfigUpdate();}}else{this.ioaConfig={modbus_slaves:[]};}const modbusState=localStorage.getItem('modbus_slaves_config');if(modbusState){const newModbusConfig=JSON.parse(modbusState);const modbusConfigChanged=JSON.stringify(this.modbusConfig)!==JSON.stringify(newModbusConfig);this.modbusConfig=newModbusConfig;if(modbusConfigChanged&&this.modbusConfig){this.triggerConfigUpdate();}}else{this.modbusConfig={slaves:[]};}}saveState(){const state={autoStatus:document.getElementById('autoStatus').checked,autoSoe:document.getElementById('autoSoe').checked,autoLogs:document.getElementById('autoLogs').checked,autoScrollLogs:document.getElementById('autoScrollLogs').checked,logEnable:document.getElementById('log_enable').checked};localStorage.setItem('goesMonitorState',JSON.stringify(state));}async getLogs(){await this.delay(200);return{logs:this.logs.slice(-100)};}async getSOE(){await this.delay(150);return{soe:this.soe};}async getStatus(){await this.delay(100);this.status=this.generateMockStatus();return this.status;}async clearLogs(){await this.delay(100);this.logs=[];return{success:true};}async clearSOE(){await this.delay(100);this.soe=[];return{success:true};}delay(ms){return new Promise(resolve=>setTimeout(resolve,ms));}}
const api=new MockAPI();
function displayStatus(status){if(!status)return;const grid=document.getElementById('statusGrid');if(!grid)return;grid.innerHTML='';const systemHeader=document.createElement('h3');systemHeader.textContent='System Status';systemHeader.style.gridColumn='1 / -1';systemHeader.style.margin='0 0 15px';systemHeader.style.paddingBottom='8px';systemHeader.style.borderBottom='2px solid #ddd';systemHeader.style.color='#333';grid.appendChild(systemHeader);if(status.system){const systemCard=createStatusCard('System',[['Uptime',status.system.uptime||'Unknown'],['Free Memory',status.system.memory_free||'Unknown'],['CPU Usage',status.system.cpu_usage||'Unknown'],['Temperature',status.system.temperature||'Unknown']]);grid.appendChild(systemCard);}if(status.esp32){const esp32Items=[['Chip Model',status.esp32.chip_model||'ESP32'],['Chip Revision',status.esp32.chip_revision||'Unknown'],['Flash Size',status.esp32.flash_size||'Unknown'],['Core Count',status.esp32.core_count||'2'],['CPU Frequency',(status.esp32.cpu_freq||240)+' MHz'],['Heap Free',status.esp32.heap_free||'Unknown'],['Stack Free',status.esp32.stack_free||'Unknown'],['Reset Reason',status.esp32.reset_reason||'Unknown']];grid.appendChild(createStatusCard('ESP32 Monitoring',esp32Items));}if(status.communication){const commItems=[['Type',status.communication.type||'Unknown'],['Status',status.communication.status||'Unknown','comm_status'],['IP Address',status.communication.ip_address||'N/A']];if(status.communication.type==='WiFi'){commItems.push(['SSID',status.communication.ssid||'Unknown']);commItems.push(['RSSI',status.communication.rssi||'Unknown']);commItems.push(['Gateway',status.communication.gateway||'Unknown']);}else if(status.communication.type==='Ethernet'){commItems.push(['Speed',status.communication.speed||'Unknown']);commItems.push(['Duplex',status.communication.duplex||'Unknown']);commItems.push(['Gateway',status.communication.gateway||'Unknown']);}else if(status.communication.type&&(status.communication.type.includes('SIM')||status.communication.type.includes('EC25')||status.communication.type.includes('7600'))){commItems.push(['Operator',status.communication.operator||'Unknown']);commItems.push(['Signal (CSQ)',status.communication.signal_strength||'0']);commItems.push(['Network',status.communication.network_type||'Unknown']);}grid.appendChild(createStatusCard('Communication',commItems));}if(status.protocols){const iec104Items=[['Status',status.protocols.iec104_status||'Unknown','iec104_status'],['Last Contact',status.protocols.iec104_last_contact||'Never'],['Common Address',status.protocols.iec104_common_addr||'1'],['K Parameter',status.protocols.iec104_k||'12'],['W Parameter',status.protocols.iec104_w||'8'],['Connection Mode','RTU to SCADA'],['Frame Sent',(status.protocols.iec104_frames_sent||0)+' frames'],['Frame Received',(status.protocols.iec104_frames_received||0)+' frames']];grid.appendChild(createStatusCard('IEC104 Protocol',iec104Items));const modbusSlaves=status.protocols.modbus_slaves||[];const modbusItems=[['Mode','ESP32 as Master'],['Status',status.protocols.modbus_status||'Unknown','modbus_status'],['Slave Count',modbusSlaves.length+' configured'],['Baud Rate',status.protocols.modbus_baud||'9600'],['Parity',status.protocols.modbus_parity||'None'],['Poll Interval',(status.protocols.modbus_poll_ms||1000)+'ms'],['Success Rate',(status.protocols.modbus_polls_success||0)+'%'],['Last Poll',status.protocols.modbus_last_poll||'Never']];if(modbusSlaves.length>0){modbusSlaves.forEach(slave=>{const slaveName=slave.name?`${slave.name} (ID:${slave.id})`:`Slave ${slave.id}`;const statusClass=slave.status==='Online'?'success':slave.status==='Offline'?'warning':'error';modbusItems.push([slaveName,slave.status||'Unknown',statusClass]);});}grid.appendChild(createStatusCard('Modbus Protocol',modbusItems));}renderFeatureTables(status);}function renderFeatureTables(status){if(!status)return;const mainBody=byId('mainFeaturesBody');const mainEmpty=byId('mainFeaturesEmpty');if(mainBody){mainBody.innerHTML='';let count=0;if(status.main_features){if('gfd'in status.main_features){addFeatureRow(mainBody,'GFD (Ground Fault Detection)','1001','Single Point Information',status.main_features.gfd);count++;}if('supply'in status.main_features){addFeatureRow(mainBody,'Power Supply','1002','Double Point Information',status.main_features.supply);count++;}if('cb_1'in status.main_features){addFeatureRow(mainBody,'Circuit Breaker 1','1003','Double Point Information',status.main_features.cb_1);count++;}if('cb_2'in status.main_features){addFeatureRow(mainBody,'Circuit Breaker 2','1004','Double Point Information',status.main_features.cb_2);count++;}if('cb_3'in status.main_features){addFeatureRow(mainBody,'Circuit Breaker 3','1005','Double Point Information',status.main_features.cb_3);count++;}}if(mainEmpty)mainEmpty.style.display=count?'none':'block';}const extBody=byId('extFeaturesBody');const extEmpty=byId('extFeaturesEmpty');if(extBody){extBody.innerHTML='';let count=0;if(status.extended_features){if('door'in status.extended_features){addFeatureRow(extBody,'Door Sensor','2001','Single Point Information',status.extended_features.door);count++;}if('di_1'in status.extended_features){addFeatureRow(extBody,'Digital Input 1','2002','Single Point Information',status.extended_features.di_1);count++;}if('di_2'in status.extended_features){addFeatureRow(extBody,'Digital Input 2','2003','Single Point Information',status.extended_features.di_2);count++;}if('di_3'in status.extended_features){addFeatureRow(extBody,'Digital Input 3','2004','Single Point Information',status.extended_features.di_3);count++;}if('di_4'in status.extended_features){addFeatureRow(extBody,'Digital Input 4','2005','Single Point Information',status.extended_features.di_4);count++;}if('di_5'in status.extended_features){addFeatureRow(extBody,'Digital Input 5','2006','Single Point Information',status.extended_features.di_5);count++;}if('temperature_sensor'in status.extended_features){addFeatureRow(extBody,'Temperature Sensor','4001','Measured Value - Scaled',status.extended_features.temperature_sensor);count++;}if('humidity_sensor'in status.extended_features){addFeatureRow(extBody,'Humidity Sensor','4002','Measured Value - Scaled',status.extended_features.humidity_sensor);count++;}}if(extEmpty)extEmpty.style.display=count?'none':'block';}const modbusBody=byId('modbusValuesBody');const modbusEmpty=byId('modbusValuesEmpty');if(modbusBody){modbusBody.innerHTML='';let rows=0;const slaves=status.protocols?.modbus_slaves||[];slaves.forEach((slave,slaveIndex)=>{const slaveName=slave.name||`Slave ${slave.id||slaveIndex+1}`;if(slave.registers&&slave.registers.length){slave.registers.forEach((register,regIndex)=>{const row=document.createElement('tr');if(regIndex===0){const td=document.createElement('td');td.style.border='1px solid #ccc';td.style.padding='6px 8px';td.textContent=slaveName;td.rowSpan=slave.registers.length;row.appendChild(td);}const nameCell=document.createElement('td');nameCell.style.border='1px solid #ccc';nameCell.style.padding='6px 8px';nameCell.textContent=register.name||`Register ${register.address}`;row.appendChild(nameCell);const addrCell=document.createElement('td');addrCell.style.border='1px solid #ccc';addrCell.style.padding='6px 8px';addrCell.style.textAlign='center';addrCell.textContent=register.address||'N/A';row.appendChild(addrCell);const typeCell=document.createElement('td');typeCell.style.border='1px solid #ccc';typeCell.style.padding='6px 8px';typeCell.style.textAlign='center';typeCell.style.fontFamily='monospace';typeCell.style.fontSize='11px';typeCell.style.color='#666';typeCell.textContent=deriveModbusType(register);row.appendChild(typeCell);const valCell=document.createElement('td');valCell.style.border='1px solid #ccc';valCell.style.padding='6px 8px';valCell.style.textAlign='center';valCell.textContent=register.value!==undefined?register.value:'N/A';row.appendChild(valCell);modbusBody.appendChild(row);rows++;});}});if(modbusEmpty)modbusEmpty.style.display=rows?'none':'block';}}
function deriveModbusType(register){const addr=parseInt(register.address)||0;if(addr>=1&&addr<=9999)return'MODBUS_COIL';if(addr>=10001&&addr<=19999)return'MODBUS_INPUT';if(addr>=30001&&addr<=39999)return'MODBUS_INPUT_REG';if(addr>=40001&&addr<=49999)return'MODBUS_HOLDING';return'MODBUS_HOLDING';}
function addFeatureRow(tbody,name,ioa,dataType,value){const row=document.createElement('tr');const nameCell=document.createElement('td');nameCell.style.border='1px solid #ddd';nameCell.style.padding='8px';nameCell.textContent=name;const ioaCell=document.createElement('td');ioaCell.style.border='1px solid #ddd';ioaCell.style.padding='8px';ioaCell.style.textAlign='center';ioaCell.style.fontFamily='monospace';ioaCell.style.fontSize='12px';ioaCell.style.color='#333';ioaCell.textContent=ioa;const dataTypeCell=document.createElement('td');dataTypeCell.style.border='1px solid #ddd';dataTypeCell.style.padding='8px';dataTypeCell.style.textAlign='center';dataTypeCell.style.fontFamily='monospace';dataTypeCell.style.fontSize='11px';dataTypeCell.style.color='#666';dataTypeCell.textContent=dataType;const valueCell=document.createElement('td');valueCell.style.border='1px solid #ddd';valueCell.style.padding='8px';valueCell.style.textAlign='center';valueCell.style.fontWeight='bold';valueCell.textContent=value;row.appendChild(nameCell);row.appendChild(ioaCell);row.appendChild(dataTypeCell);row.appendChild(valueCell);tbody.appendChild(row);}
function createStatusCard(title,items){const card=document.createElement('div');card.className='status-card';const header=document.createElement('h4');header.textContent=title;card.appendChild(header);items.forEach(([label,value,type])=>{const item=document.createElement('div');item.style.marginBottom='6px';const labelSpan=document.createElement('span');labelSpan.textContent=label+': ';labelSpan.style.fontSize='12px';labelSpan.style.color='#666';const valueSpan=document.createElement('span');valueSpan.className='status-value';if(type==='digital'){valueSpan.textContent=value?'ON':'OFF';valueSpan.classList.add(value?'on':'off');}else if(type==='cb'){valueSpan.textContent=value;valueSpan.classList.add(value==='CLOSED'?'on':'off');}else if(type==='comm_status'){valueSpan.textContent=value;if(value==='Connected'||value==='Link Up'){valueSpan.classList.add('on');}else{valueSpan.classList.add('off');}}else if(type==='iec104_status'){valueSpan.textContent=value;if(value==='Connected'){valueSpan.classList.add('on');}else{valueSpan.classList.add('fault');}}else if(type==='modbus_status'){valueSpan.textContent=value;if(value==='Master Active'){valueSpan.classList.add('on');}else{valueSpan.classList.add('off');}}else if(type==='success'){valueSpan.textContent=value;valueSpan.classList.add('on');}else if(type==='warning'){valueSpan.textContent=value;valueSpan.classList.add('off');}else if(type==='error'){valueSpan.textContent=value;valueSpan.classList.add('fault');}else{valueSpan.textContent=value;}item.appendChild(labelSpan);item.appendChild(valueSpan);card.appendChild(item);});return card;}
async function loadSOE(){try{const data=await api.getSOE();const tbody=document.getElementById('soeBody');tbody.innerHTML='';if(data.soe&&data.soe.length>0){data.soe.reverse().forEach(entry=>{const row=tbody.insertRow();row.innerHTML=`<td style='padding:3px 6px'>${entry.seq}</td><td style='padding:3px 6px'>${entry.millis}</td><td style='padding:3px 6px'>${entry.ioa}</td><td style='padding:3px 6px'>${entry.val}</td>`;});document.getElementById('soeSummary').textContent=`${data.soe.length} events`;}else{const row=tbody.insertRow();row.innerHTML=`<td colspan='4' style='text-align:center;padding:8px;color:#999'>No SOE data</td>`;document.getElementById('soeSummary').textContent='0 events';}}catch(error){showNotification('Error loading SOE: '+error.message,'error');}}
async function clearSOE(){try{await api.clearSOE();await loadSOE();showNotification('SOE cleared successfully','success');}catch(error){showNotification('Error clearing SOE: '+error.message,'error');}}
async function loadLogs(){if(!document.getElementById('log_enable').checked){document.getElementById('logBox').textContent='Logs disabled. Enable logs to view output.';return;}try{const data=await api.getLogs();const logBox=document.getElementById('logBox');const wasScrolledToBottom=logBox.scrollHeight-logBox.clientHeight<=logBox.scrollTop+1;if(data.logs&&data.logs.length>0){logBox.textContent=data.logs.join('\n');if(document.getElementById('autoScrollLogs').checked&&wasScrolledToBottom){logBox.scrollTop=logBox.scrollHeight;}}else{logBox.textContent='No log data available';}}catch(error){showNotification('Error loading logs: '+error.message,'error');}}
async function clearLogs(){try{await api.clearLogs();await loadLogs();showNotification('Logs cleared successfully','success');}catch(error){showNotification('Error clearing logs: '+error.message,'error');}}
async function loadStatus(){try{api.loadConfigState();const status=await api.getStatus();displayStatus(status);updateLiveStatusPanel();updateRtuClockBar();renderFeatureTables(status);}catch(error){showNotification('Error loading status: '+error.message,'error');}}
function showNotification(message,type='success'){const status=document.getElementById('status');status.textContent=message;status.className=`status ${type}`;status.style.display='block';setTimeout(()=>{status.style.display='none';},3000);}
function updateLiveStatusPanel(){if(!api||!api.status)return;const status=api.status;const panel=byId('liveStatusPanel');if(panel){panel.classList.add('prototype');}byId('ls_conn').textContent='Online';byId('ls_comm').textContent=status.communication?.type||api.config?.comm_method||'-';byId('ls_uptime').textContent=status.system?.uptime||'0d 0h 0m';byId('ls_heap').textContent=status.system?.memory_free||'45.2 KB';byId('ls_stack').textContent='1.2K';byId('ls_stack_comm').textContent='856';byId('ls_stack_iec104').textContent='724';byId('ls_stack_hal').textContent='512';byId('ls_rev').textContent='prototype-v1.0';if(status.protocols?.iec104_frames_sent!==undefined)byId('ls_iec104_tx').textContent=status.protocols.iec104_frames_sent;if(status.protocols?.iec104_frames_received!==undefined)byId('ls_iec104_rx').textContent=status.protocols.iec104_frames_received;if(status.protocols?.iec104_last_contact)byId('ls_iec104_last').textContent=status.protocols.iec104_last_contact.replace('s ago','');}
function updateRtuClockBar(){if(!api||!api.status)return;const st=api.status.system||{};const full=byId('rtu_time_full');if(full)full.textContent=st.rtu_time||'--';const src=byId('rtu_time_source');if(src)src.textContent='Source: '+(st.rtu_time_source||'RTC');const upd=byId('rtu_time_updated');if(upd){const d=new Date();upd.textContent=d.toLocaleTimeString();}}
let statusInterval,soeInterval,logsInterval;function setupAutoRefresh(){if(statusInterval)clearInterval(statusInterval);if(document.getElementById('autoStatus').checked){statusInterval=setInterval(loadStatus,5000);}if(soeInterval)clearInterval(soeInterval);if(document.getElementById('autoSoe').checked){soeInterval=setInterval(loadSOE,3000);}if(logsInterval)clearInterval(logsInterval);if(document.getElementById('autoLogs').checked){logsInterval=setInterval(loadLogs,2000);}}
document.addEventListener('DOMContentLoaded',function(){loadStatus();loadSOE();loadLogs();updateLiveStatusPanel();updateRtuClockBar();if(api&&api.status)renderFeatureTables(api.status);setupAutoRefresh();setInterval(function(){api.loadConfigState();api.loadIOAConfig();},10000);window.addEventListener('storage',function(e){if(e.key==='goesConfigState'){api.loadConfigState();loadStatus();showNotification('Configuration updated - status refreshed','info');}else if(e.key==='prototype_ioa_config'||e.key==='modbus_slaves_config'){api.loadIOAConfig();loadStatus();showNotification('Modbus/IOA configuration updated - status refreshed','info');}});window.addEventListener('focus',function(){api.loadConfigState();api.loadIOAConfig();loadStatus();});document.getElementById('btnRefreshStatus').addEventListener('click',loadStatus);document.getElementById('btnRefreshConfig').addEventListener('click',function(){api.loadConfigState();loadStatus();showNotification('Configuration reloaded from localStorage','success');});document.getElementById('btnRefreshSoe').addEventListener('click',loadSOE);document.getElementById('btnClearSoe').addEventListener('click',clearSOE);document.getElementById('btnRefreshLogs').addEventListener('click',loadLogs);document.getElementById('btnClearLogs').addEventListener('click',clearLogs);document.getElementById('autoStatus').addEventListener('change',function(){setupAutoRefresh();api.saveState();});document.getElementById('autoSoe').addEventListener('change',function(){setupAutoRefresh();api.saveState();});document.getElementById('autoLogs').addEventListener('change',function(){setupAutoRefresh();api.saveState();});document.getElementById('log_enable').addEventListener('change',function(){if(this.checked){loadLogs();}else{document.getElementById('logBox').textContent='Logs disabled. Enable logs to view output.';}api.saveState();});document.getElementById('autoScrollLogs').addEventListener('change',function(){api.saveState();});document.getElementById('btnApplyLogFilter').addEventListener('click',function(){showNotification('Log filters applied (prototype mode)','success');});document.getElementById('btnFilterComm').addEventListener('click',function(){document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);['log_cat_comm','log_cat_ethernet','log_cat_modem','log_cat_wifi'].forEach(id=>{const el=document.getElementById(id);if(el)el.checked=true;});showNotification('Communication filter applied','success');});document.getElementById('btnFilterErrors').addEventListener('click',function(){document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);document.getElementById('log_cat_error').checked=true;showNotification('Error filter applied','success');});document.getElementById('btnFilterProtocol').addEventListener('click',function(){document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);['log_cat_iec104','log_cat_modbus'].forEach(id=>{const el=document.getElementById(id);if(el)el.checked=true;});showNotification('Protocol filter applied','success');});document.getElementById('btnFilterSystem').addEventListener('click',function(){document.querySelectorAll('.logCat').forEach(cb=>cb.checked=false);['log_cat_system','log_cat_config','log_cat_task'].forEach(id=>{const el=document.getElementById(id);if(el)el.checked=true;});showNotification('System filter applied','success');});});
function byId(id){return document.getElementById(id);}function showNotification(msg,type='info'){const n=byId('notif');if(!n)return;if(n.hideTimeout){clearTimeout(n.hideTimeout);delete n.hideTimeout;}n.textContent=msg;if(type===true)type='error';else if(type===false)type='success';n.className='notif-bar '+type;n.style.display='block';const hideDelay=(type==='error')?8000:5000;n.hideTimeout=setTimeout(()=>{n.style.display='none';delete n.hideTimeout;},hideDelay);}window.addEventListener('beforeunload',function(){if(statusInterval)clearInterval(statusInterval);if(soeInterval)clearInterval(soeInterval);if(logsInterval)clearInterval(logsInterval);});
</script>
</script>
</body></html>
)HTML";

#endif // WEB_PAGE_H
