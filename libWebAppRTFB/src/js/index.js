import { initializeApp } from 'firebase/app';
import { getAuth, signInWithEmailAndPassword, createUserWithEmailAndPassword  } from 'firebase/auth';
import { getDatabase } from 'firebase/database';
import '../styles/style.css';

const firebaseConfig = {
  apiKey: "AIzaSyAG2ogN6DiJ8EJbh5sTSwHCPPPIWXod3ZE",
  authDomain: "library-seating-system.firebaseapp.com",
  databaseURL: "https://library-seating-system-default-rtdb.firebaseio.com",
  projectId: "library-seating-system",
  storageBucket: "library-seating-system.appspot.com",
  messagingSenderId: "329372628077",
  appId: "1:329372628077:web:6620e71f80f51c947fbb64",
  measurementId: "G-V6F704DM9E"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);
const db = getDatabase(app);


document.getElementById('login-form').addEventListener('submit', function(e) {
    e.preventDefault();

    const email = document.getElementById('login-email').value; // Use 'login-email' here
    const password = document.getElementById('login-password').value; // Use 'login-password' here

    signInWithEmailAndPassword(auth, email, password)
        .then((userCredential) => {
            // Signed in
            window.location.href = 'home.html'; // Redirect to home page
        })
        .catch((error) => {
            // Error occurred
            console.error('Login failed:', error.message);
            // Display error message to the user
        });
});



// Handle Signup
document.getElementById('signup-form').addEventListener('submit', (e) => {
    e.preventDefault();

    const email = document.getElementById('signup-email').value;
    const password = document.getElementById('signup-password').value;
    const passwordError = document.getElementById('password-error');

    if (password.length < 6) {
        // Show custom error message
        passwordError.style.display = 'block';
    } else {
        passwordError.style.display = 'none';

        createUserWithEmailAndPassword(auth, email, password)
            .then((userCredential) => {
                // Signup successful
                console.log('Account created:', userCredential.user);
                window.location.href = 'home.html';
            })
            .catch((error) => {
                console.error('Signup failed:', error.message);
                // Optionally, show Firebase error message
            });
    }
});


