const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta charset="UTF-8">
    <title>ESP32 DFPlayer Mini Web Server</title>
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
        height: 100%;
      }

      body {
        margin: 0;
        display: flex;
        flex-direction: column;
        height: 100%;
      }

      h1 { font-size: 2.0rem; color:#2980b9; }

      .content {
        /* flex: auto;  */
        overflow: auto;
        height: 400px;
      }

      .content ul {
        list-style-type: none;
        padding: 0;
        margin: 0;
        max-height: 400px;
        overflow-y: auto;
        border: 1px solid #ccc;
        border-radius: 5px;
      }

      .content li {
        padding: 10px;
        margin: 5px;
        background-color: #f0f0f0;
        cursor: pointer;
        border-radius: 5px;
      }
      .content li:hover {
        background-color: #ddd;
      }

      .fixed-footer {
        position: fixed;
        bottom: 0;
        left: 0;
        width: 100%;
        background-color: #fff;
        padding: 15px 0;
        box-shadow: 0 -2px 5px rgba(0, 0, 0, 0.2);
        z-index: 1000;
      }

      .fixed-footer .button {
        display: inline-block;
        padding: 5px 20px;
        margin-left: 15px;
        margin-right: 15px;
        margin-bottom: 15px;
        font-size: 30px;
        font-weight: bold;
        cursor: pointer;
        text-align: center;
        text-decoration: none;
        outline: none;
        color: #fff;
        border: none;
        border-radius: 12px;
        box-shadow: 0 2.5px #999;
      }
      .fixed-footer .button:active {
        box-shadow: 0 1px #666;
        transform: translateY(2px);
      }

      .fixed-footer .button-volumeDown {background-color: #0a97b0; font-size: 15px;}
      .fixed-footer .button-volumeDown:hover {background-color: #0b889e;}

      .fixed-footer .button-volumeUp {background-color: #0a97b0; font-size: 15px;}
      .fixed-footer .button-volumeUp:hover {background-color: #0b889e;}

      .fixed-footer .button-repeat {background-color: #1b6ca8; font-size: 20px;}
      .fixed-footer .button-repeat:hover {background-color: #185b8f;}

      .fixed-footer .button-prev {background-color: #1b6ca8;}
      .fixed-footer .button-prev:hover {background-color: #185b8f;}
      .fixed-footer .button-next {background-color: #1b6ca8;}
      .fixed-footer .button-next:hover {background-color: #185b8f;}

      .fixed-footer .button-toggle { background-color: #f8b24f; }
      .fixed-footer .button-toggle:hover {background-color: #dda049;}
    </style>
  </head>

  <body>
    <div class="content">
      <h1>ESP32C3 Music Player</h1>
      <ul>
        <li onclick="playSong('8', 'Road So Far')">Road So Far</li>
        <li onclick="playSong('1', 'Một Vòng Việt Nam')">Một Vòng Việt Nam</li>
        <li onclick="playSong('2', 'Nhìn Kìa Trời Tối Rồi')">Nhìn Kìa Trời Tối Rồi</li>
        <li onclick="playSong('3', 'Jingle Bells')">Jingle Bells</li>
        <li onclick="playSong('4', 'Happy New Year')">Happy New Year</li>
        <li onclick="playSong('5', 'Một Năm Mới Bình An')">Một Năm Mới Bình An</li>
        <li onclick="playSong('6', 'Tiến Lên Việt Nam Ơi')">Tiến Lên Việt Nam Ơi</li>
        <li onclick="playSong('7', 'Cheri Cheri Lady')">Cheri Cheri Lady</li>
      </ul>
    </div>

    <div class="fixed-footer">
        <button class="button button-prev" onclick="sendData('PV')"> &#9198; </button>
  
        <button id="toggleIcon" class="button button-toggle" onclick="togglePlayPause()"> &#9654; </button>

        <button class="button button-next" onclick="sendData('NT')"> &#9197; </button>

        <br><br>
        <button class="button button-volumeDown" onclick="sendData('VD')">VOLUME -</button>
        <button class="button button-volumeUp" onclick="sendData('VU')">VOLUME +</button>
        <button id="togglerepeat" class="button button-repeat" onclick="toggleRepeat()">&#10227;</button>

      <p id="play_status">Status : Please wait...</p>
      <p id="volume">Volume : Please wait...</p>
    </div>

    <script>
      function playSong(songNumber, songName) {
        // Cập nhật trạng thái hiển thị tên bài hát
        document.getElementById("play_status").innerText = `Status: Playing - ${songName}`;
        
        // Gửi lệnh phát bài hát
        const sobaihat = songNumber.toString();
        sendData(sobaihat);
      }
      
      let isPlaying = false; // Bắt đầu ở trạng thái chưa phát
      // Hàm toggle giữa Pause và Play/Continue
      function togglePlayPause() {
        const toggleIcon = document.getElementById("toggleIcon");
        const playStatus = document.getElementById("play_status");
        
        if (isPlaying) {
          sendData('PE'); // Gửi lệnh Pause
          toggleIcon.innerHTML = "&#9654;"; // Biểu tượng Pause
          playStatus.innerHTML = "Status: Paused";
        } else {
          sendData('PY'); // Gửi lệnh Play
          toggleIcon.innerHTML = "&#9208;" // Biểu tượng Play
          playStatus.innerHTML = "Status: Playing";
        }
        isPlaying = !isPlaying; // Đảo trạng thái
      }

      let isRepeating = false; // Bắt đầu không Repeat
      // Hàm toggle giữa Repeat và No Repeat
      function toggleRepeat() {
        const togglerepeat = document.getElementById("togglerepeat");
        const playStatus = document.getElementById("play_status");

        // Kiểm tra trạng thái hiện tại của play (Playing hay Paused)
        const currentStatus = playStatus.innerHTML;
        
        if (!isRepeating) {
          sendData('RP'); // Gửi lệnh Repeat
          if (currentStatus.includes("Playing")) {
            playStatus.innerHTML = "Status: Playing - Repeat"; 
          } else {
            playStatus.innerHTML = "Status: Paused - Repeat"; 
          }
        } else {
          sendData('NRP'); // Gửi lệnh không Repeat
          if (currentStatus.includes("Playing")) {
            playStatus.innerHTML = "Status: Playing - No Repeat"; 
          } else {
            playStatus.innerHTML = "Status: Paused - No Repeat"; 
          }
        }
        isRepeating = !isRepeating; // Đảo trạng thái
      }

      function sendData(CMD) {
        const xmlhttp = new XMLHttpRequest();
        xmlhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            const response = this.responseText.split(",");
            if (response[0] == "Data") {
              document.getElementById("play_status").innerText = 
                response[1] == "1" ? "Status: Playing" : 
                response[1] == "2" ? "Status: Paused" : "Status: Playing";
              document.getElementById("volume").innerText = "Volume : " + response[2];
            } else if (response[0] == "Sta") {
              document.getElementById("play_status").innerText = 
                response[1] == "1" ? "Status: Playing" : 
                response[1] == "2" ? "Status: Paused" : "Status: Playing";
            } else if (response[0] == "Vlm") {
              document.getElementById("volume").innerText = "Volume : " + response[1]; 
            }
          }
        };
        xmlhttp.open("GET", "setPLAYER?PLAYERCMD=" + CMD, true);
        xmlhttp.send(); 
      }
    </script>
  </body>
</html>


)=====";