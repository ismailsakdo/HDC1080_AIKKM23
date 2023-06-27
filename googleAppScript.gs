function doGet(e) { 
  Logger.log( JSON.stringify(e) );
  var result = 'Ok';
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
else {
  var sheet_id = 'idgoglesheet'; // Spreadsheet ID
  var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();
  var newRow = sheet.getLastRow() + 1; 
  var rowData = [];
  var Curr_Date = new Date();
  rowData[0] = Curr_Date; // Date in column A
  var Curr_Time = Utilities.formatDate(Curr_Date, "Asia/Singapore", 'HH:mm:ss');
  rowData[1] = Curr_Time; // Time in column B
  for (var param in e.parameter) {
    Logger.log('In for loop, param=' + param);
    var value = stripQuotes(e.parameter[param]);
    Logger.log(param + ':' + e.parameter[param]);
    switch (param) {
      case 'temperature':
        rowData[2] = value; // Temperature
        result = 'Temp Written on column C'; 
        break;
      case 'humidity':
        rowData[3] = value; // Humidity
        result += ' ,Humidity Written on column D'; 
        break;
      case 'thi':
        rowData[4] = value; // THI
        result += ' ,THI Written on column D'; 
        break;
      case 'status':
        rowData[5] = value; // status
        result += ' ,Status'; 
        break; 
      case 'latlong':
        rowData[6] = value; // status
        result += ' ,Status'; 
        break; 
      default:
        result = "unsupported parameter";
    }
  }
  Logger.log(JSON.stringify(rowData));
  var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
  newRange.setValues([rowData]);
}
  return ContentService.createTextOutput(result);
}
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}


function sendEmail(){
  var ss = SpreadsheetApp.getActiveSpreadsheet();
  var sheet = ss.getSheetByName("Sheet1");
  
  var lastRow = sheet.getLastRow();
  var lastColumn = sheet.getLastColumn();
  var maxRow = sheet.getMaxRows();
  var maxCol = sheet.getMaxColumns();
  
  //var accZ = sheet.getRange(lastRow, lastColumn-1).getValue();
  var lastCell = sheet.getRange(lastRow-1, lastColumn).getValue();
  Logger.log(lastCell);
  
  if(lastCell=="Poor")
  {
    GmailApp.sendEmail("mywataverse@gmail.com", "Alert", "Humidex Trigger, check temperature and humidity in the location. Alert 1 Activated");
  } 
}
