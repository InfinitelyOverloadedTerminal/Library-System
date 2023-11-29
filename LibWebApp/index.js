// Import the functions you need from the SDKs you need
const { initializeApp } = require ("firebase/app");
//const { getAnalytics } = require ("firebase/analytics");
const { getFirestore, collection, addDoc, getDocs, limit, query, orderBy } = require ("firebase/firestore");
const { Timestamp } = require("firebase/firestore");

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
    apiKey: "AIzaSyCbPLwzBHnTBEFFhCYackQkvL5GBFM4Ypw",
  authDomain: "web-socket-assignment.firebaseapp.com",
  projectId: "web-socket-assignment",
  storageBucket: "web-socket-assignment.appspot.com",
  messagingSenderId: "646636994536",
  appId: "1:646636994536:web:92015aba03b637f44c2e5d"
    //measurementId: "G-60NS54V647"
  };
  // Initialize Firebase
const firebaseApp = initializeApp(firebaseConfig);
const firestore = getFirestore(firebaseApp);

var express = require('express');
var socket = require('socket.io');


// App Setup
var app = express();
var server = app.listen(4000,function(){
    console.log('listening to request on port 4000');
});

// Static Files
app.use(express.static('public'));

// Socket Setup
var io = socket(server);

io.on('connection', async function(socket){
    console.log('made socket connection', socket.id);

    // On Connect
    try {
        // Query the collection for the 10 most recent messages
        const messagesRef = collection(firestore, 'messages');
        const messagesQuery = query(messagesRef, orderBy('timestamp', 'asc'), limit(10));
        const messagesSnapshot = await getDocs(messagesQuery);

        // Emit each of the 10 most recent messages
        messagesSnapshot.forEach((doc) => {
            io.to(socket.id).emit('chat', doc.data());
        });
    } catch (error) {
        console.error('Error retrieving messages:', error);
    }

    socket.on('chat', async function(data){
        try{
            // Add a new document in the "messages" collection with username and message
            const messagesRef = collection(firestore, 'messages');
            const docRef = await addDoc(messagesRef, {
                handle: data.handle,
                message: data.message,
                timestamp: new Date() // Add a timestamp for ordering
            });

            console.log('Document added with ID:', docRef.id);

            io.sockets.emit('chat', data);
        } catch (error) {
            console.error('Error adding message:', error);
        }
        
    });

    socket.on('typing', function(data){
        socket.broadcast.emit('typing', data)
    })
});