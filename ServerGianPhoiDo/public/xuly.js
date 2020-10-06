var socket = io("http://localhost:3000");

// var socket = io('/webapp');

var led = []
var ledJSON = {"led": led}
var s, hoten, msg

// socket.on('server_gui_dataMessage', function(data){
// 	data.each(function(i,val){
// 		hoten = "<span class='hoten'>" + i.user + ": </span>"
// 		msg = "<span class='msg'>" + i.message + "</span>"
// 		$('#dsMsg').append(hoten + msg + "<div class='block'></div>")
// 	})
// })

socket.on('server_gui_DHT11', function(data){
	// alert(data.Humidity + ' : ' + data.Temperature)
	$('#Temperature').empty()
	$('#Humidity').empty()
	$('#Temperature').append(data.Temperature+'&#8451;')
	$('#Humidity').append(data.Humidity+'%')
})

socket.on('svgtChat', function(data){
	var hoten = "<span class='hoten'>" + data.username + ": </span>"
	var msg = "<span class='msg'>" + data.message + "</span>"
	$('#dsMsg').append(hoten + msg + "<div class='block'></div>")
})

socket.on('server_gui_kqua', function(data){

	if (!data.ketqua) {
		alert('Đăng ký thành công Username: ' + data.user)
		$('#tenUserdaDK').show(1000)
		$('#txtMessage').show(1000)
		$('#btnSendChat').show(1000)
		$('#txtUser').hide(1000)
		$('#btnDangKy').hide(1000)
	} else {
		alert('Đã tồn tại Username: ' + data.user + '. Mời bạn đăng ký một tên khác nha...')
	}
})

socket.on('server_gui_ds_user_name', function(data){
	$('#danhsachUsersOnline').empty()
	for (var i = 0; i < data.danhsach.length; i++) {
		var dsuser = "<div class='motUser'>"+data.danhsach[i]+"</div>"
		$('#danhsachUsersOnline').append(dsuser);
	}
})

socket.on('server_gui_tenUserdaDK', function(data){
	$('#tenUserdaDK').text(data.tenUserDaDK)
})

socket.on('STORE_LED', function(data){

	led[0] = data.led[0];
	led[1] = data.led[1];
	led[2] = data.led[2];

	var status

	if(data.led[0]){

		status = "<div class='onOffRed'><h4>Bật</h4></div>"
		$('#trangThai1').empty()
		$('#trangThai1').append(status)

		// $('#trangThai1').text('Bật')
		$("#toggle1").prop("checked", true);
		$("#myonoffswitch1").prop("checked", true);
		$("#switch1").prop("checked", true);

		// s = "<input class='toggle toggle-ios' id='toggle1' type='checkbox' checked/><label class='toggle-btn' for='toggle1'></label><br>"

		// $('#btnDo').empty()
		// $('#btnDo').append(s)
	} else {

		status = "<div class='onOffRed'><h4>Tắt</h4></div>"
		$('#trangThai1').empty()
		$('#trangThai1').append(status)

		// $('#trangThai1').text('Tắt')
		$("#toggle1").prop("checked", false);
		$("#myonoffswitch1").prop("checked", false);
		$("#switch1").prop("checked", false);

		// s = "<input class='toggle toggle-ios' id='toggle1' type='checkbox'/><label class='toggle-btn' for='toggle1'></label><br>"

		// $('#btnDo').empty()
		// $('#btnDo').append(s)
	}

	if(data.led[1]){

		status = "<div class='onOffGreen'><h4>Bật</h4></div>"
		$('#trangThai2').empty()
		$('#trangThai2').append(status)

		// $('#trangThai2').text('Bật')
		$("#toggle2").prop("checked", true);
		$("#myonoffswitch2").prop("checked", true);
		$("#switch2").prop("checked", true);

		// s = "<input class='toggle toggle-ios' id='toggle2' type='checkbox' checked/><label class='toggle-btn' for='toggle2'></label><br>"

		// $('#btnXanh').empty()
		// $('#btnXanh').append(s)
	} else {

		status = "<div class='onOffGreen'><h4>Tắt</h4></div>"
		$('#trangThai2').empty()
		$('#trangThai2').append(status)

		// $('#trangThai2').text('Tắt')
		$("#toggle2").prop("checked", false);
		$("#myonoffswitch2").prop("checked", false);
		$("#switch2").prop("checked", false);

		// s = "<input class='toggle toggle-ios' id='toggle2' type='checkbox'/><label class='toggle-btn' for='toggle2'></label><br>"

		// $('#btnXanh').empty()
		// $('#btnXanh').append(s)
	}

	if(data.led[2]){

		status = "<div class='onOffYellow'><h4>Bật</h4></div>"
		$('#trangThai3').empty()
		$('#trangThai3').append(status)

		// $('#trangThai3').text('Bật')
		$("#toggle3").prop("checked", true);
		$("#myonoffswitch3").prop("checked", true);
		$("#switch3").prop("checked", true);

		// s = "<input class='toggle toggle-ios' id='toggle3' type='checkbox' checked/><label class='toggle-btn' for='toggle3'></label><br>"

		// $('#btnVang').empty()
		// $('#btnVang').append(s)
	} else {

		status = "<div class='onOffYellow'><h4>Tắt</h4></div>"
		$('#trangThai3').empty()
		$('#trangThai3').append(status)

		// $('#trangThai3').text('Tắt')
		$("#toggle3").prop("checked", false);
		$("#myonoffswitch3").prop("checked", false);
		$("#switch3").prop("checked", false);

		// s = "<input class='toggle toggle-ios' id='toggle3' type='checkbox'/><label class='toggle-btn' for='toggle3'></label><br>"

		// $('#btnVang').empty()
		// $('#btnVang').append(s)
	}
})

socket.on('server_gui_LIGHT', function(data){

	var s;
	if(data.digital){
		s = "<div class='motGiaTri'><h4>It's dark</h4></div>"
	} else {
		s = "<div class='motGiaTri'><h4>In broad daylight</h4></div>"
	}

	// $('#camBienAS').remove()
	$('#camBienAS').empty()
	
	$('#camBienAS').append(s)
})

socket.on('LED_STATUS', function(data){
	ledDo = data.redStatus;
	ledXanh = data.ledXanh;
	ledVang = data.yellowStatus;

	if(ledDo){
		$('#trangThai1').text('Bật')
	} else {
		$('#trangThai1').text('Tắt')
	}

	if(ledXanh){
		$('#trangThai2').text('Bật')
	} else {
		$('#trangThai2').text('Tắt')
	}

	if(ledVang){
		$('#trangThai3').text('Bật')
	} else {
		$('#trangThai3').text('Tắt')
	}
})

$(document).ready(function(){

	$('#den1').on('click', function() {
		led[0] = !led[0]
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		
	});
	$('#den2').on('click', function() {
		led[1] = !led[1]
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	});
	$('#den3').on('click', function() {
		led[2] = !led[2]
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	});

	$('#capnhat').on('click', function(){
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#btnchat').on('click', function(){
		socket.emit('chatWebapp')
	})

	$('#toggle1').click(function(){

		$("#toggle1").css("background-color", "red");

		if ($("input[id='toggle1']").prop('checked') == true) {
			led[0] = true
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		} else {
			led[0] = false
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		}
	})

	$('#toggle2').on('click', function(){

		$("#toggle2").css("background-color", "green");

		if ($("input[id='toggle2']").prop('checked') == true) {
			led[1] = true
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		} else {
			led[1] = false
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		}
	})

	$('#toggle3').on('click', function(){

		$("#toggle3").css("background-color", "yellow");

		if ($("input[id='toggle3']").prop('checked') == true) {
			led[2] = true
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		} else {
			led[2] = false
			socket.emit('dieu_khien_led_tu_webapp', ledJSON)
		}
	})

	$('#myonoffswitch1').on('click', function(){
		if ($("input[id='myonoffswitch1']").prop('checked') == true) {
			led[0] = true
		} else {
			led[0] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#myonoffswitch2').on('click', function(){
		if ($("input[id='myonoffswitch2']").prop('checked') == true) {
			led[1] = true
		} else {
			led[1] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#myonoffswitch3').on('click', function(){
		if ($("input[id='myonoffswitch3']").prop('checked') == true) {
			led[2] = true
		} else {
			led[2] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#switch1').on('click', function(){
		if ($("input[id='switch1']").prop('checked') == true) {
			led[0] = true
		} else {
			led[0] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#switch2').on('click', function(){
		if ($("input[id='switch2']").prop('checked') == true) {
			led[1] = true
		} else {
			led[1] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	$('#switch3').on('click', function(){
		if ($("input[id='switch3']").prop('checked') == true) {
			led[2] = true
		} else {
			led[2] = false
		}
		socket.emit('dieu_khien_led_tu_webapp', ledJSON)
	})

	// var $on = 'section';
 //    $($on).css({
 //      'background':'none',
 //      'border':'none',
 //      'box-shadow':'none'
 //    });
    

	// $("[name='cks']").change(function(){
 //    var str="";
 //    $("input[name='cks']:checked").each(function(i,val){
 //      str += $(this).val() + " duoc checked, ";
 //    });
 //    $("#cks_div").html(str);
 //    });


	// Demo Chat
	$('#tenUserdaDK').hide()
	$('#chat').hide();
	$('#btnControl').hide()
	$('#txtMessage').hide()
	$('#btnSendChat').hide()

	$('#btnchat').click(function(){
		$('#chat').show(2000);
		$('#btnchat').hide();
		$('#capnhat').hide()
		$('#dieukhien').hide(1000)
		$('#btnControl').show()
	})
	$('#btnControl').click(function(){
		$('#chat').hide(1000);
		$('#btnchat').show();
		$('#capnhat').show()
		$('#dieukhien').show(2000)
		$('#btnControl').hide()
	})
	$('#btnDangKy').click(function(){
		socket.emit('client_dangky_user_name', $('#txtUser').val())
	})

	$("#btnSendChat").click(function(){
		socket.emit('client_gui_tinChat', $('#txtMessage').val())
		$('#txtMessage').val('')
	})

});