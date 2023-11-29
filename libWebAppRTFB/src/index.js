import { initializeApp } from "firebase/app";
import { getDatabase, ref, set, update, onValue, get} from "firebase/database";

const firebaseConfig = {
    apiKey: "AIzaSyD7ERj1W04Bdo8oymV8KOxLbQDIxXkLyU4",
    authDomain: "esp32-communication-test.firebaseapp.com",
    databaseURL: "https://esp32-communication-test-default-rtdb.firebaseio.com",
    projectId: "esp32-communication-test",
    storageBucket: "esp32-communication-test.appspot.com",
    messagingSenderId: "655675116449",
    appId: "1:655675116449:web:38145ddc763758af7559e0",
    measurementId: "G-X5X6N9T403"
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



