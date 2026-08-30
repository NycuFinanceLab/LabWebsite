const SPREADSHEET_ID = '14fRU2aa-4rmwAj1IaozAZiuX9w58O5moH8XiJcLwFJM';
const SHEET_GID = 691164893;
const PHOTO_FOLDER_ID = '1bkciIy-gicyePfGMrVPqaPAPmsD0_o47';

const REQUIRED_HEADERS = [
  'name',
  'title',
  'group',
  'email',
  'research',
  'photo_file',
  'profile_url',
  'order',
  'visible',
];

function doGet(e) {
  let payload;

  try {
    payload = getStudentsPayload_();
  } catch (error) {
    payload = {
      ok: false,
      errors: [error.message],
      requiredHeaders: REQUIRED_HEADERS,
      actualHeaders: [],
      students: [],
      updatedAt: new Date().toISOString(),
    };
  }

  const output = JSON.stringify(payload);
  const callback = e && e.parameter && e.parameter.callback;

  if (callback) {
    const callbackName = String(callback).replace(/[^\w.$]/g, '');

    return ContentService
      .createTextOutput(`${callbackName}(${output});`)
      .setMimeType(ContentService.MimeType.JAVASCRIPT);
  }

  return ContentService
    .createTextOutput(output)
    .setMimeType(ContentService.MimeType.JSON);
}

function getStudentsPayload_() {
  const sheet = getSheetByGid_(SPREADSHEET_ID, SHEET_GID);
  const values = sheet.getDataRange().getDisplayValues();

  if (values.length === 0) {
    return {
      ok: false,
      errors: ['Sheet is empty.'],
      requiredHeaders: REQUIRED_HEADERS,
      actualHeaders: [],
      students: [],
    };
  }

  const headers = values[0].map((header) => String(header).trim());
  const errors = validateHeaders_(headers);
  const rows = values.slice(1);
  const students = rows
    .map((row) => rowToObject_(headers, row))
    .filter((student) => Object.values(student).some((value) => String(value).trim() !== ''))
    .map((student) => ({
      ...student,
      photo_url: getPhotoUrl_(student.photo_file),
    }));

  return {
    ok: errors.length === 0,
    errors,
    spreadsheetId: SPREADSHEET_ID,
    sheetGid: SHEET_GID,
    photoFolderId: PHOTO_FOLDER_ID,
    requiredHeaders: REQUIRED_HEADERS,
    actualHeaders: headers,
    students,
    updatedAt: new Date().toISOString(),
  };
}

function getSheetByGid_(spreadsheetId, gid) {
  const spreadsheet = SpreadsheetApp.openById(spreadsheetId);
  const sheets = spreadsheet.getSheets();
  const sheet = sheets.find((candidate) => candidate.getSheetId() === Number(gid));

  if (!sheet) {
    throw new Error(`Sheet gid not found: ${gid}`);
  }

  return sheet;
}

function validateHeaders_(headers) {
  const actual = headers.slice(0, REQUIRED_HEADERS.length).map((header) => header.toLowerCase());
  const expected = REQUIRED_HEADERS.map((header) => header.toLowerCase());
  const mismatches = expected.filter((header, index) => actual[index] !== header);

  if (mismatches.length === 0) {
    return [];
  }

  return [
    `Header row must exactly be: ${REQUIRED_HEADERS.join(' | ')}`,
    `Actual header row is: ${headers.join(' | ')}`,
  ];
}

function rowToObject_(headers, row) {
  return headers.reduce((record, header, index) => {
    if (header) {
      record[header] = row[index] || '';
    }

    return record;
  }, {});
}

function getPhotoUrl_(photoFile) {
  if (!PHOTO_FOLDER_ID || !photoFile) {
    return '';
  }

  const files = DriveApp.getFolderById(PHOTO_FOLDER_ID).getFilesByName(photoFile);

  if (!files.hasNext()) {
    return '';
  }

  return `https://drive.google.com/uc?export=view&id=${files.next().getId()}`;
}
