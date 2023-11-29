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

// get all key with "Table" in name
function fetchAndListenToTables() {
  const tablesRef = ref(db, 'Tables');

  onValue(tablesRef, (snapshot) => {
    const allTables = snapshot.val();
    if (allTables) {
      Object.keys(allTables).forEach((tableId) => {
        if (tableId.startsWith('Table')) {
          // Assuming you're looking for nodes that start with 'Table'
          const tableData = allTables[tableId];
          // update table info and set state to false in DB
          const tableRef = ref(db, `Tables/${tableId}`);
          updateTableDisplay(tableId, tableData);
        }
      });
    }
  });
}

function updateTableDisplay(tableId, tableData) {
  const tablesList = document.querySelector('.table-list');

  // Check if the table is already displayed
  let tableItem = document.getElementById(`table-${tableId}`);
  if (!tableItem) {
    // Create a new list item for the table if it doesn't exist
    tableItem = document.createElement('li');
    tableItem.id = `table-${tableId}`;
    tablesList.appendChild(tableItem);
  }

  // Add 'table-item' class and additional class based on reserved status
  tableItem.className = `table-item ${tableData.reserved ? 'reserved' : 'available'}`;

  // Generate HTML for each seat
  let seatsHtml = '';
  for (const seatId in tableData) {
    if (tableData.hasOwnProperty(seatId) && seatId.startsWith('Seat')) {
      const seat = tableData[seatId];
      seatsHtml += `<li class="seat ${seat.isAvailable ? 'available' : 'occupied'}">${seatId}</li>`;
    }
  }

  // Update the content of the table list item
  tableItem.innerHTML = `
    <h3>${tableId}</h3>
    <ul class="seats">${seatsHtml}</ul>
  `;
}


fetchAndListenToTables();
