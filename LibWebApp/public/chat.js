// make connection
var socket = io.connect('localhost:4000');
//var socket = io.connect('https://d7f2-174-64-101-41.ngrok-free.app%27/);

// Query DOM
var message = document.getElementById('message');
    handle = document.getElementById('handle'),
    btn = document.getElementById('send'),
    output = document.getElementById('output'),
    feedback = document.getElementById('feedback');

//Emitt Events

btn.addEventListener('click', function(){
    socket.emit('chat', {
        message: message.value,
        handle: handle.value
    })
});

message.addEventListener('keypress', function(){
    socket.emit('typing',handle.value)
});

// Listen for events
socket.on('chat',function(data){
    feedback.innerHTML ="";
    output.innerHTML += '<p><strong>' + data.handle + ': </strong>' + data.message + '</p>';
});

socket.on('typing', function(data){
    feedback.innerHTML = '<p><em>' + data +' is typing a message...</em></p>';
});