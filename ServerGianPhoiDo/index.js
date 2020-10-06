var express = require("express");
var app = express();
app.use(express.static("public"));
app.set("view engine", "ejs");
app.set("views", "./views");

var mysql = require('mysql')

var server = require("http").Server(app);
var io = require("socket.io")(server);
server.listen(process.env.PORT || 3000);

var esp8266_nsp = io.of('/esp8266')				//namespace của esp8266

//Bắt các sự kiện khi esp8266 kết nối
esp8266_nsp.on('connection', function(socket) {
	console.log('esp8266 connected')
	
	socket.on('disconnect', function() {
		console.log("Disconnect socket esp8266")
	})
	
	//nhận được bất cứ lệnh nào
	socket.on("*", function(packet) {
		console.log("esp8266 rev and send to webapp packet: ", packet.data) //in ra để debug
		var eventName = packet.data[0]
		var eventJson = packet.data[1] || {} //nếu gửi thêm json thì lấy json từ lệnh gửi, không thì gửi chuỗi json rỗng, {}
		webapp_nsp.emit(eventName, eventJson) //gửi toàn bộ lệnh + json đến webapp
	})
})

//Khi có mệt kết nối được tạo giữa Socket Client và Socket Server
io.on('connection', function(socket) {	
	//hàm console.log giống như hàm Serial.println trên Arduino
    console.log("Connected : " + socket.id); //In ra màn hình console là đã có một Socket Client kết nối thành công.

    socket.emit('ardOi')

	socket.on('dieu_khien_led_tu_webapp', function(data){

		led[0] = Boolean(data.led[0])
		led[1] = Boolean(data.led[1])
		led[2] = Boolean(data.led[2])
		console.log(json)

		io.sockets.emit('STORE_LED', json)
	})

	socket.on('android_gui_dieuKhienDC', function(data){
		console.log('android_gui_dieuKhienDC: '+ data)
		io.sockets.emit('svdkDC', { "dieukhienDC": data })
		console.log('da gui arduino: dieuKhienDC')
	})

	socket.on('android_gui_Control', function(data){
		console.log('android_gui_Control: '+ data)
		io.sockets.emit('svgCT', { "status": data })
		console.log('da gui arduino: Control')
	})

	socket.on('STATUSDC', function(data){
		console.log('STATUSDC: '+ JSON.stringify(data))
		io.sockets.emit('server_gui_trang_thai_DC', data)
		console.log('da gui cho android: STATUSDC')
	})

	socket.on('LIGHT', function(data){
		console.log(data.message)
		io.sockets.emit('server_gui_LIGHT', data)

	})

	socket.on('DHT11', function(data){
		console.log(data.Humidity + ' : ' + data.Temperature)

		io.sockets.emit('server_gui_DHT11', data)
	})

	socket.on('STATECT', function(data){
		console.log('STATECT: ' + data.stateCT)

		io.sockets.emit('server_gui_STATECT', data)
	})

	//Khi socket client bị mất kết nối thì chạy hàm sau.
	socket.on('disconnect', function() {
		console.log("disconnect : " +socket.id) 	//in ra màn hình console cho vui
	})
})