import { initializeApp } from "firebase/app";
import { getDatabase, ref, set, update, onValue, get} from "firebase/database";

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
const db = getDatabase(app);

// Update progress bar
function updateProgressBar(value) {
  var newValue = ((value / 4095) * 100).toFixed(2);

  const displaySpan = document.getElementById("ldrVal");
  var progressBar = document.getElementById("ldrRange");
  displaySpan.textContent = value;

  // Update the width and aria-valuenow attribute
  progressBar.style.width = newValue + "%";
  progressBar.setAttribute('aria-valuenow', newValue);
  progressBar.textContent = newValue;
}
// get ldr value from database on change, update progress bar
function getLDRValue() {
  const reference = ref(db, "ldr/value");

  onValue(reference, (snapshot) => {
    const data = snapshot.val();
    updateProgressBar(data);
  });
}
getLDRValue();

//Update LED state
document.getElementById('ledStatusButton').addEventListener('click', function() {
  const ledRef = ref(db, 'ldr/state');
  
  get(ledRef).then((snapshot) => {
    const isLedOn = snapshot.val();

    const led = ref(db, 'ldr/');
    update(led, { state: !isLedOn });
    
    updateLedStatusButton(!isLedOn);
  });
});

// get ldr value from database on change, update slider
function getLEDState() {
  // Listen for changes in LED state from Firebase
  const ledRef = ref(db, 'ldr/state');
  onValue(ledRef, (snapshot) => {
    const isLedOn = snapshot.val();
    updateLedStatusButton(isLedOn);
  });
}
getLEDState();

function updateLedStatusButton(isLedOn) {
  const button = document.getElementById('ledStatusButton');
  if (isLedOn) {
      button.textContent = 'LED is On';
      button.classList.remove('btn-secondary');
      button.classList.add('btn-success');
  } else {
      button.textContent = 'LED is Off';
      button.classList.remove('btn-success');
      button.classList.add('btn-secondary');
  }
}

// Now update LED brightness
document.getElementById('ledRange').addEventListener('change', function() {
  const value = this.value;
  const led = ref(db, 'ldr/');
  update(led, { intensity: parseInt(value) });
});






