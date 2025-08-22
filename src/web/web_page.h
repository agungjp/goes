#ifndef WEB_PAGE_H
#define WEB_PAGE_H

// Redesigned page with feature toggles
const char WEB_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html lang='en'>
<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1.0'>
<title>GOES Configurator</title>
<style>
 body{font-family:Arial,Helvetica,sans-serif;margin:0;background:#f2f5f9;color:#222}
 header{background:#1a3d6d;color:#fff;padding:14px 24px}
 h1{margin:0;font-size:20px;letter-spacing:.5px}
 main{padding:18px;max-width:1180px;margin:0 auto}
 h2{margin:28px 0 12px;font-size:17px;color:#1a3d6d}
 fieldset{border:1px solid #d4dbe4;margin:0 0 18px;padding:14px 16px;border-radius:8px;background:#fff}
 legend{padding:0 6px;font-weight:600;font-size:13px;color:#1a3d6d}
 label{display:block;margin:0 0 4px;font-weight:600;font-size:12px;letter-spacing:.3px;text-transform:uppercase;color:#425b76}
 input[type=text],input[type=number],select{width:100%;padding:8px 10px;border:1px solid #b8c4d1;border-radius:6px;font-size:14px;background:#fff;box-sizing:border-box}
 input[type=checkbox]{transform:scale(1.15);margin-right:6px}
 button{background:#0078d4;color:#fff;padding:10px 20px;border:none;border-radius:6px;font-size:14px;font-weight:600;cursor:pointer;box-shadow:0 2px 4px rgba(0,0,0,.15)}
 button:hover{background:#0062ad}
 .grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(210px,1fr));gap:14px}
 .two-col{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:18px}
 .muted{font-size:11px;color:#667}
 .status{margin-top:15px;padding:10px;border-radius:6px;display:none;font-size:13px;font-weight:600}
 .status.success{background:#e1f6e8;color:#0f6a2b}
 .status.error{background:#fde7e9;color:#a4262c}
 #liveStatusPanel{margin-top:18px;font-size:13px;padding:10px 12px;border:1px solid #99b;background:#e9f1fb;border-radius:8px;line-height:1.5;display:flex;flex-wrap:wrap;gap:14px;align-items:center}
 #liveStatusPanel.offline{background:#fdeeee;border-color:#f2b8b5}
 #featureToggles .toggle-item{display:flex;align-items:center;gap:6px;margin:4px 0;font-size:13px}
 #featureToggles .toggle-item span{flex:1}
 .group-label{font-size:11px;font-weight:600;color:#555;margin-top:6px;text-transform:uppercase;letter-spacing:.5px}
 footer{padding:14px 24px;text-align:center;font-size:11px;color:#678;background:#f0f4f8;margin-top:40px;border-top:1px solid #d5dce3}
 pre#logBox{background:#111;color:#0f0;padding:10px;height:260px;overflow:auto;font-size:11px;border-radius:6px;border:1px solid #333}
</style></head>
<body><header><h1>GOES Configurator</h1></header><main>
<section id='device-section'><h2>Device Configuration</h2>
<form id='device-form'><div class='two-col'>
<fieldset><legend>Communication & WiFi</legend>
 <div class='grid'>
  <div class='form-group'><label for='comm_method'>Communication</label><select id='comm_method' name='comm_method'><option value='modem'>Modem</option><option value='ethernet'>Ethernet</option></select></div>
  <div class='form-group'><label for='wifi_ssid'>WiFi SSID</label><input type='text' id='wifi_ssid' name='wifi_ssid'></div>
  <div class='form-group'><label for='wifi_password'>WiFi Password</label><input type='text' id='wifi_password' name='wifi_password'></div>
 </div>
 <div id='modem-settings' class='grid' style='margin-top:8px'>
  <div class='form-group'><label for='apn'>APN</label><input type='text' id='apn' name='apn'></div>
  <div class='form-group'><label for='apn_user'>APN User</label><input type='text' id='apn_user' name='apn_user'></div>
  <div class='form-group'><label for='apn_pass'>APN Pass</label><input type='text' id='apn_pass' name='apn_pass'></div>
 </div>
 <p class='muted'>Isi APN sesuai operator. User/pass kosong jika tidak diperlukan.</p>
</fieldset>
<fieldset><legend>Feature Toggles</legend><div id='featureToggles'>
 <div class='group-label'>Core Groups</div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_digital_inputs'> <span>Digital Inputs</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_circuit_breakers'> <span>Circuit Breakers</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_digital_outputs'> <span>Digital Outputs</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_watchdog'> <span>Task Watchdog</span></label></div>
 <div class='group-label'>Granular</div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_gfd'> <span>GFD</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_supply'> <span>Supply Status</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_remote'> <span>Remote 1-3</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_cb1'> <span>CB1</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_cb2'> <span>CB2</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_cb3'> <span>CB3</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_temp'> <span>Temperature</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_bms'> <span>BMS</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_di_cadangan'> <span>DI Cadangan</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_do_cadangan'> <span>DO Cadangan</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_ethernet'> <span>Ethernet Option</span></label></div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_rtc'> <span>RTC</span></label></div>
 <div class='group-label'>Web / Debug</div>
 <div class='toggle-item'><label><input type='checkbox' id='feat_web_log'> <span>Web Log Capture</span></label></div>
</div><p class='muted'>Centang untuk mengaktifkan. Simpan untuk persist.</p></fieldset>
</div><button type='submit'>Save Device Config</button></form></section>
<hr>
<section id='ioa-section'><h2>IOA Configuration</h2>
<form id='ioa-form'>
 <fieldset><legend>Digital Inputs (M_SP_NA_1)</legend><div class='grid'>
  <div><label for='di_gfd'>GFD</label><input type='number' id='di_gfd' name='di_gfd'></div>
  <div><label for='di_supply_status'>Supply Status</label><input type='number' id='di_supply_status' name='di_supply_status'></div>
  <div><label for='di_remote_1'>Remote 1</label><input type='number' id='di_remote_1' name='di_remote_1'></div>
  <div><label for='di_remote_2'>Remote 2</label><input type='number' id='di_remote_2' name='di_remote_2'></div>
  <div><label for='di_remote_3'>Remote 3</label><input type='number' id='di_remote_3' name='di_remote_3'></div>
  <div><label for='di_cadangan_1'>Cadangan 1</label><input type='number' id='di_cadangan_1' name='di_cadangan_1'></div>
  <div><label for='di_cadangan_2'>Cadangan 2</label><input type='number' id='di_cadangan_2' name='di_cadangan_2'></div>
  <div><label for='di_cadangan_3'>Cadangan 3</label><input type='number' id='di_cadangan_3' name='di_cadangan_3'></div>
 </div></fieldset>
 <fieldset><legend>Circuit Breakers (Status / Command)</legend><div class='grid'>
  <div><label for='status_cb_1'>Status CB1</label><input type='number' id='status_cb_1' name='status_cb_1'></div>
  <div><label for='status_cb_2'>Status CB2</label><input type='number' id='status_cb_2' name='status_cb_2'></div>
  <div><label for='status_cb_3'>Status CB3</label><input type='number' id='status_cb_3' name='status_cb_3'></div>
  <div><label for='command_cb_1'>Command CB1</label><input type='number' id='command_cb_1' name='command_cb_1'></div>
  <div><label for='command_cb_2'>Command CB2</label><input type='number' id='command_cb_2' name='command_cb_2'></div>
  <div><label for='command_cb_3'>Command CB3</label><input type='number' id='command_cb_3' name='command_cb_3'></div>
 </div></fieldset>
 <fieldset><legend>Digital Outputs (C_SC_NA_1)</legend><div class='grid'>
  <div><label for='do_cadangan_1'>DO Cadangan 1</label><input type='number' id='do_cadangan_1' name='do_cadangan_1'></div>
  <div><label for='do_cadangan_2'>DO Cadangan 2</label><input type='number' id='do_cadangan_2' name='do_cadangan_2'></div>
 </div></fieldset>
 <button type='submit'>Save IOA Config</button>
</form></section>
<div id='status' class='status'></div>
<div id='liveStatusPanel'>
 <span id='ls_conn'>Polling...</span>
 <span>| Comm: <strong id='ls_comm'>-</strong></span>
 <span>| Uptime: <span id='ls_uptime'>-</span></span>
 <span>| Heap: <span id='ls_heap'>-</span></span>
 <span>| Stack(main): <span id='ls_stack'>-</span></span>
 <span>| Stack(comm): <span id='ls_stack_comm'>-</span></span>
 <span>| Stack(iec104): <span id='ls_stack_iec104'>-</span></span>
 <span>| Stack(hal): <span id='ls_stack_hal'>-</span></span>
 <span>| Rev: <code id='ls_rev'>-</code></span>
</div>
<section style='padding:12px' id='logs-section'>
 <h2>Logs</h2>
 <div style='display:flex;gap:8px;align-items:center;margin-bottom:6px'>
  <button type='button' id='btnRefreshLogs'>Refresh</button>
  <label style='font-size:12px'><input type='checkbox' id='autoLogs' checked> Auto (2s)</label>
 </div>
 <pre id='logBox'></pre>
</section>
</main><footer>GOES &copy; 2025</footer>
<script>
let lastStatus=null,initialRev=null,expectingRestart=false,pollIntervalMs=1200;let logTimer=null;
document.addEventListener('DOMContentLoaded',()=>{loadConfig();byId('comm_method').addEventListener('change',toggleCommSections);byId('device-form').addEventListener('submit',onDeviceSubmit);byId('ioa-form').addEventListener('submit',onIoaSubmit);startStatusPolling();initLogs();});
function initLogs(){const b=byId('btnRefreshLogs');if(b)b.addEventListener('click',loadLogs);const a=byId('autoLogs');if(a)a.addEventListener('change',()=>{if(a.checked){scheduleLogs();}else if(logTimer){clearTimeout(logTimer);}});scheduleLogs();loadLogs();}
function scheduleLogs(){if(logTimer)clearTimeout(logTimer);const a=byId('autoLogs');if(!a||!a.checked)return;logTimer=setTimeout(()=>{loadLogs();scheduleLogs();},2000);} 
function loadLogs(){fetch('/api/logs').then(r=>r.json()).then(j=>{const box=byId('logBox');if(!box)return;box.textContent=(j.lines||[]).join('\n');box.scrollTop=box.scrollHeight;});}
function loadConfig(){fetch('/api/config2').then(r=>{if(!r.ok) throw 0; return r.json();}).then(d=>{const dv=(d.device&&typeof d.device==='object')?d.device:parseJSON(d.device||'{}');const io=(d.ioa&&typeof d.ioa==='object')?d.ioa:parseJSON(d.ioa||'{}');applyDeviceConfig(dv);applyIoaConfig(io);}).catch(()=>{fetch('/api/config').then(r=>r.json()).then(d=>{const dv=(d.device&&typeof d.device==='object')?d.device:parseJSON(d.device||'{}');const io=(d.ioa&&typeof d.ioa==='object')?d.ioa:parseJSON(d.ioa||'{}');applyDeviceConfig(dv);applyIoaConfig(io);});});}
function parseJSON(s){try{return JSON.parse(s);}catch(e){return {};}}
function applyDeviceConfig(dv){setVal('comm_method',dv.comm_method||'modem');['wifi_ssid','wifi_password','apn','apn_user','apn_pass'].forEach(k=>setVal(k,dv[k]||''));['feat_digital_inputs','feat_circuit_breakers','feat_digital_outputs','feat_watchdog','feat_gfd','feat_supply','feat_remote','feat_cb1','feat_cb2','feat_cb3','feat_temp','feat_bms','feat_di_cadangan','feat_do_cadangan','feat_ethernet','feat_rtc','feat_web_log'].forEach(id=>setToggle(id,dv[id]!==0));if(!byId('comm_method').getAttribute('data-original'))byId('comm_method').setAttribute('data-original',dv.comm_method||'modem');toggleCommSections();maybeHideEthernet(dv);} 
function maybeHideEthernet(dv){if(dv.feat_ethernet===0){const opt=[...byId('comm_method').options].find(o=>o.value==='ethernet');if(opt)opt.style.display='none';if(byId('comm_method').value==='ethernet'){byId('comm_method').value='modem';}}}
function applyIoaConfig(io){['di_gfd','di_supply_status','di_remote_1','di_remote_2','di_remote_3','di_cadangan_1','di_cadangan_2','di_cadangan_3','status_cb_1','status_cb_2','status_cb_3','command_cb_1','command_cb_2','command_cb_3','do_cadangan_1','do_cadangan_2'].forEach(k=>setVal(k,io[k]||0));}
function onDeviceSubmit(e){e.preventDefault();const data=formToJSON(e.target);['feat_digital_inputs','feat_circuit_breakers','feat_digital_outputs','feat_watchdog','feat_gfd','feat_supply','feat_remote','feat_cb1','feat_cb2','feat_cb3','feat_temp','feat_bms','feat_di_cadangan','feat_do_cadangan','feat_ethernet','feat_rtc','feat_web_log'].forEach(id=>data[id]=byId(id).checked?1:0);const oldMethod=byId('comm_method').getAttribute('data-original')||'';if(oldMethod&&oldMethod!==data.comm_method)expectingRestart=true;saveConfig('/api/config/device',data);} 
function onIoaSubmit(e){e.preventDefault();const data=formToJSON(e.target);['di_gfd','di_supply_status','di_remote_1','di_remote_2','di_remote_3','di_cadangan_1','di_cadangan_2','di_cadangan_3','status_cb_1','status_cb_2','status_cb_3','command_cb_1','command_cb_2','command_cb_3','do_cadangan_1','do_cadangan_2'].forEach(k=>data[k]=parseInt(data[k]||'0',10));saveConfig('/api/config/ioa',data);} 
function byId(id){return document.getElementById(id);}function setVal(id,v){const el=byId(id);if(el)el.value=v;}function setToggle(id,on){const el=byId(id);if(el)el.checked=!!on;}function toggleCommSections(){byId('modem-settings').style.display=byId('comm_method').value==='modem'?'grid':'none';}
function formToJSON(f){return Object.fromEntries(new FormData(f).entries());}
function saveConfig(url,data){fetch(url,{method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(data)}).then(r=>r.text().then(t=>({ok:r.ok,text:t}))).then(o=>{if(/Restarting/i.test(o.text))expectingRestart=true;showStatus(o.text,!o.ok);}).catch(e=>showStatus('Error: '+e,true));}
function showStatus(msg,err){const st=byId('status');st.textContent=msg;st.className='status '+(err?'error':'success');st.style.display='block';setTimeout(()=>st.style.display='none',6500);} 
function startStatusPolling(){pollStatus();setInterval(pollStatus,pollIntervalMs);} 
function pollStatus(){fetch('/api/status').then(r=>r.json()).then(s=>{updateLiveStatus(s);if(lastStatus){if(s.rev!==lastStatus.rev){showStatus('Revision: '+s.rev,false);}if(lastStatus.uptime>=5 && s.uptime+3<lastStatus.uptime){showStatus('Device restarted (reason: '+(s.reset_reason||'?')+')',false);} }lastStatus=s;if(initialRev===null)initialRev=s.rev;}).catch(setOffline);} 
function updateLiveStatus(s){const panel=byId('liveStatusPanel');panel.classList.remove('offline');byId('ls_conn').textContent='Online';byId('ls_comm').textContent=s.comm_method||'-';byId('ls_uptime').textContent=formatUptime(s.uptime||0);byId('ls_heap').textContent=s.heap_free||'-';byId('ls_stack').textContent=s.main_task_stack_free_words||'-';byId('ls_stack_comm').textContent=s.comm_task_stack_free_words||'-';byId('ls_stack_iec104').textContent=s.iec104_task_stack_free_words||'-';byId('ls_stack_hal').textContent=s.hal_task_stack_free_words||'-';byId('ls_rev').textContent=s.rev||'-';if(expectingRestart&&lastStatus&&s.uptime<8){showStatus('Restart complete (Comm='+s.comm_method+')',false);expectingRestart=false;}}
function setOffline(){const p=byId('liveStatusPanel');p.classList.add('offline');['ls_conn','ls_comm','ls_uptime','ls_rev','ls_heap','ls_stack','ls_stack_comm','ls_stack_iec104','ls_stack_hal'].forEach(id=>{const el=byId(id);if(el)el.textContent='-';});byId('ls_conn').textContent='Offline';}
function formatUptime(sec){const h=Math.floor(sec/3600),m=Math.floor((sec%3600)/60),s=sec%60;return (h>0?h+'h ':'')+(m>0||h>0?m+'m ':'')+s+'s';}
</script>
</body></html>
)rawliteral";

#endif // WEB_PAGE_H
