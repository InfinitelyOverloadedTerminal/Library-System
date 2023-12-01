import { initializeApp } from "firebase/app";
import { getDatabase, ref, set, update, onValue, get, remove } from "firebase/database";

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



// get all key with "Table" in name
function fetchAndListenToTables() {
  const tablesRef = ref(db, 'Tables');

  onValue(tablesRef, (snapshot) => {
    const allTables = snapshot.val();
    if (allTables) {
      Object.keys(allTables).forEach((tableId) => {
        if (tableId.startsWith('Table ')) {
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

  let tableItem = document.getElementById(`table-${tableId}`);
  if (!tableItem) {
    tableItem = document.createElement('li');
    tableItem.classList.add('table-item');
    tableItem.id = `table-${tableId}`;
    tablesList.appendChild(tableItem);
  }

  tableItem.className = tableData.reserved ? 'table-item red-table' : 'table-item';

  let seatsHtml = '';
  for (const seatId in tableData) {
    if (seatId !== 'reserved') {
      const seatValue = tableData[seatId];
      // Check if the seat value is 0 and adjust the display accordingly
      seatsHtml += `<li class="seat">${seatValue === 0 ? 'Empty' : `ID: ${seatValue}`}</li>`;
    }
  }

  // Add a delete button to each table
  tableItem.innerHTML = `
    <h3>${tableId}</h3>
    <ul class="seats">${seatsHtml}</ul>
  `;

  const deleteButton = document.createElement('button');
  deleteButton.textContent = 'Delete All Seats';
  deleteButton.style.backgroundColor = '#f44336'; // Red color
  deleteButton.style.color = 'white';
  deleteButton.style.border = 'none';
  deleteButton.style.padding = '10px 20px';
  deleteButton.style.borderRadius = '4px';
  deleteButton.style.cursor = 'pointer';
  deleteButton.style.marginTop = '10px';
  deleteButton.textContent = 'Delete All Seats';
  deleteButton.addEventListener('click', function() {
    deleteTableSeats(tableId);
  });

  // Append the delete button to the tableItem
  tableItem.appendChild(deleteButton);

}

function deleteTableSeats(tableId) {
  const tableRef = ref(db, `Tables/${tableId}`);

  get(tableRef).then((snapshot) => {
    const tableData = snapshot.val();
    if (tableData) {
      const updates = {};
      Object.keys(tableData).forEach((seatId) => {
        if (seatId !== 'reserved') {
          updates[`${tableId}/${seatId}`] = 0;
        }
      });

      // Perform the update
      if (Object.keys(updates).length > 0) {
        update(ref(db, 'Tables'), updates)
          .then(() => {
            console.log('Seats updated to "empty" successfully');
          })
          .catch((error) => {
            console.error("Error updating seats: ", error);
          });
      }
    }
  }).catch((error) => {
    console.error("Error fetching table data: ", error);
  });
}



fetchAndListenToTables();
