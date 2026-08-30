const navToggle = document.querySelector(".nav-toggle");
const siteNav = document.querySelector("#site-nav");

if (navToggle && siteNav) {
  navToggle.addEventListener("click", () => {
    const isOpen = siteNav.classList.toggle("is-open");
    navToggle.setAttribute("aria-expanded", String(isOpen));
  });

  siteNav.addEventListener("click", (event) => {
    if (event.target instanceof HTMLAnchorElement) {
      siteNav.classList.remove("is-open");
      navToggle.setAttribute("aria-expanded", "false");
    }
  });
}

const searchInput = document.querySelector("#resource-search");
const resourceList = document.querySelector("[data-resource-list]");

if (searchInput && resourceList) {
  const resources = Array.from(resourceList.querySelectorAll("a"));

  searchInput.addEventListener("input", () => {
    const query = searchInput.value.trim().toLowerCase();

    resources.forEach((resource) => {
      const haystack = `${resource.textContent || ""} ${resource.dataset.tags || ""}`.toLowerCase();
      resource.classList.toggle("is-hidden", query !== "" && !haystack.includes(query));
    });
  });
}

const facebookMarquee = document.querySelector("[data-facebook-marquee]");

if (facebookMarquee) {
  const setMarqueePost = (link) => {
    const title = (link.textContent || "").trim();
    const href = link.getAttribute("href");

    if (!title || !href) {
      return;
    }

    facebookMarquee.href = href;
    facebookMarquee.querySelectorAll("[data-facebook-marquee-text]").forEach((item) => {
      item.textContent = title;
    });
  };

  const latestFacebookPost = (root) => {
    const markedPost = root.querySelector("[data-latest-facebook-post]");

    if (markedPost) {
      return markedPost;
    }

    return Array.from(root.querySelectorAll("a[href*='facebook.com']")).find((link) => {
      const href = link.getAttribute("href") || "";
      return href.includes("/posts/") || href.includes("/share/p/");
    });
  };

  const localPost = latestFacebookPost(document);

  if (localPost && localPost !== facebookMarquee) {
    setMarqueePost(localPost);
  } else {
    fetch(facebookMarquee.dataset.source || "resources.html")
      .then((response) => {
        if (!response.ok) {
          throw new Error(`Unable to load Facebook post source: ${response.status}`);
        }
        return response.text();
      })
      .then((html) => {
        const doc = new DOMParser().parseFromString(html, "text/html");
        const sourcePost = latestFacebookPost(doc);

        if (sourcePost) {
          setMarqueePost(sourcePost);
        }
      })
      .catch((error) => {
        console.warn(error);
      });
  }
}

document.querySelectorAll(".publication-table").forEach((table) => {
  const headerRow = table.querySelector("thead tr");

  if (headerRow && !headerRow.querySelector(".pub-year-cell")) {
    const yearHeader = document.createElement("th");
    yearHeader.scope = "col";
    yearHeader.className = "pub-year-cell";
    yearHeader.textContent = "Year";
    headerRow.prepend(yearHeader);
  }

  table.querySelectorAll("tbody tr").forEach((row) => {
    if (row.querySelector(".pub-year-cell")) {
      return;
    }

    const journalText = row.children[2]?.textContent || "";
    const parenthesizedYear = journalText.match(/\((19|20)\d{2}\)/);
    const yearMatches = journalText.match(/\b(19|20)\d{2}\b/g);
    const fallbackYear = yearMatches && yearMatches.length > 0 ? yearMatches[yearMatches.length - 1] : "-";
    const year = row.dataset.year || parenthesizedYear?.[0].replace(/[()]/g, "") || fallbackYear;
    const yearCell = document.createElement("td");
    yearCell.className = "pub-year-cell";
    yearCell.textContent = year;
    row.prepend(yearCell);
  });
});

const visibleSheetRows = (rows) => {
  const visibleRows = rows.filter((row) => {
    const value = String(row.visible ?? "").trim().toLowerCase();
    return value === "" || ["true", "yes", "y", "1"].includes(value);
  });

  return visibleRows.sort((a, b) => {
    const orderA = Number(a.order);
    const orderB = Number(b.order);

    if (Number.isFinite(orderA) && Number.isFinite(orderB)) {
      return orderA - orderB;
    }

    if (Number.isFinite(orderA)) {
      return -1;
    }

    if (Number.isFinite(orderB)) {
      return 1;
    }

    return String(a.name || "").localeCompare(String(b.name || ""), "zh-Hant");
  });
};

const fetchJsonp = (url) => new Promise((resolve, reject) => {
  const callbackName = `sheetCallback_${Date.now()}_${Math.random().toString(36).slice(2)}`;
  const script = document.createElement("script");
  const cleanup = () => {
    delete window[callbackName];
    script.remove();
  };

  window[callbackName] = (payload) => {
    cleanup();
    resolve(payload);
  };

  script.onerror = () => {
    cleanup();
    reject(new Error("Apps Script JSONP request failed."));
  };

  const requestUrl = new URL(url);
  requestUrl.searchParams.set("callback", callbackName);
  requestUrl.searchParams.set("cacheBust", Date.now());
  script.src = requestUrl.toString();
  document.head.append(script);
});

const loadAppsScriptData = async (apiUrl) => {
  try {
    const requestUrl = new URL(apiUrl);
    requestUrl.searchParams.set("cacheBust", Date.now());
    const response = await fetch(requestUrl.toString());

    if (!response.ok) {
      throw new Error(`Apps Script returned HTTP ${response.status}.`);
    }

    return await response.json();
  } catch (error) {
    console.warn("Falling back to Apps Script JSONP.", error);
    return fetchJsonp(apiUrl);
  }
};

const parseCsv = (text) => {
  const rows = [];
  let row = [];
  let field = "";
  let inQuotes = false;

  for (let index = 0; index < text.length; index += 1) {
    const char = text[index];
    const nextChar = text[index + 1];

    if (char === "\"") {
      if (inQuotes && nextChar === "\"") {
        field += "\"";
        index += 1;
      } else {
        inQuotes = !inQuotes;
      }
      continue;
    }

    if (char === "," && !inQuotes) {
      row.push(field);
      field = "";
      continue;
    }

    if ((char === "\n" || char === "\r") && !inQuotes) {
      if (char === "\r" && nextChar === "\n") {
        index += 1;
      }
      row.push(field);
      if (row.some((value) => value.trim() !== "")) {
        rows.push(row);
      }
      row = [];
      field = "";
      continue;
    }

    field += char;
  }

  row.push(field);
  if (row.some((value) => value.trim() !== "")) {
    rows.push(row);
  }

  const headers = (rows.shift() || []).map((header) => header.trim());

  return rows.map((values) => headers.reduce((record, header, index) => {
    record[header] = (values[index] || "").trim();
    return record;
  }, {}));
};

const createLinkedText = (label, href) => {
  if (!href) {
    return document.createTextNode(label);
  }

  const link = document.createElement("a");
  link.href = href;
  link.textContent = label;
  return link;
};

const researchList = document.querySelector("[data-research-list]");

if (researchList) {
  const excludedResearchTitles = new Set(
    (researchList.dataset.excludedResearchTitles || "")
      .split("|")
      .map((title) => title.trim().toLowerCase())
      .filter(Boolean),
  );

  const renderResearchCard = (item) => {
    const card = document.createElement("a");
    card.className = "feature-card";
    card.href = item.url;

    if (item.target) {
      card.target = item.target;
    }

    const label = document.createElement("span");
    label.textContent = item.category || "Research";

    const title = document.createElement("strong");
    title.textContent = `${item.prefix || ""}${item.title || "Research"}${item.suffix || ""}`;

    const note = document.createElement("small");
    note.textContent = item.note || item.category || "";

    card.append(label, title, note);
    return card;
  };

  const visibleResearchRows = (rows) => rows
    .filter((row) => {
      const visible = String(row.visible ?? "").trim().toLowerCase();
      const title = String(row.title || "").trim();
      return (visible === "" || ["true", "yes", "y", "1"].includes(visible))
        && title
        && row.url
        && !excludedResearchTitles.has(title.toLowerCase());
    })
    .sort((a, b) => {
      const sortA = Number(a.sort || a.order);
      const sortB = Number(b.sort || b.order);

      if (Number.isFinite(sortA) && Number.isFinite(sortB)) {
        return sortA - sortB;
      }

      if (Number.isFinite(sortA)) {
        return -1;
      }

      if (Number.isFinite(sortB)) {
        return 1;
      }

      return String(a.title || "").localeCompare(String(b.title || ""), "zh-Hant");
    });

  const loadResearchRows = async (url) => {
    const requestUrl = new URL(url, window.location.href);
    requestUrl.searchParams.set("cacheBust", Date.now());
    const response = await fetch(requestUrl.toString());

    if (!response.ok) {
      throw new Error(`Research CSV returned HTTP ${response.status}.`);
    }

    return visibleResearchRows(parseCsv(await response.text()));
  };

  const loadResearch = async () => {
    const primaryUrl = researchList.dataset.researchUrl || "";
    const fallbackUrl = researchList.dataset.researchFallbackUrl || "";
    const minimumCount = Number(researchList.dataset.minResearchCount || 0);
    let rows = [];

    try {
      rows = await loadResearchRows(primaryUrl);

      if (minimumCount > 0 && rows.length < minimumCount) {
        throw new Error(`Research CSV returned ${rows.length} rows; expected at least ${minimumCount}.`);
      }
    } catch (error) {
      console.warn("Unable to load complete research data from Google Sheet.", error);

      if (fallbackUrl) {
        rows = await loadResearchRows(fallbackUrl);
      }
    }

    if (rows.length > 0) {
      researchList.replaceChildren(...rows.map(renderResearchCard));
    }
  };

  loadResearch().catch((error) => {
    console.error("Unable to load research links.", error);
  });
}

const positionsList = document.querySelector("[data-positions-list]");

if (positionsList) {
  const positionsUrl = positionsList.dataset.positionsUrl || "";

  const renderPosition = (position) => {
    const item = document.createElement("li");
    const title = position.title || "Position";
    const period = position.period || [position.start_year, position.end_year].filter(Boolean).join("-");
    const heading = document.createElement("strong");
    heading.textContent = period ? `${title} (${period})` : title;

    const detail = document.createElement("span");
    const organization = position.organization || "";
    const affiliation = position.affiliation || "";
    const notes = position.notes || "";

    if (organization) {
      detail.append(createLinkedText(organization, position.organization_url));
    }

    if (affiliation) {
      if (detail.childNodes.length > 0) {
        detail.append(document.createTextNode(", "));
      }
      detail.append(createLinkedText(affiliation, position.affiliation_url));
    }

    if (notes) {
      if (detail.childNodes.length > 0) {
        detail.append(document.createTextNode(". "));
      }
      detail.append(document.createTextNode(notes));
    }

    item.append(heading, detail);
    return item;
  };

  const loadPositions = async () => {
    if (!positionsUrl) {
      return;
    }

    try {
      const requestUrl = new URL(positionsUrl);
      requestUrl.searchParams.set("cacheBust", Date.now());
      const response = await fetch(requestUrl.toString());

      if (!response.ok) {
        throw new Error(`Position CSV returned HTTP ${response.status}.`);
      }

      const positions = visibleSheetRows(parseCsv(await response.text()));

      if (positions.length === 0) {
        throw new Error("No visible positions were returned.");
      }

      positionsList.replaceChildren(...positions.map(renderPosition));
    } catch (error) {
      console.error("Unable to load positions from Google Sheet.", error);
    }
  };

  loadPositions();
}

const honorsList = document.querySelector("[data-honors-list]");

if (honorsList) {
  const honorsUrl = honorsList.dataset.honorsUrl || "";

  const appendSentencePart = (container, text) => {
    if (!text) {
      return;
    }

    if (container.childNodes.length > 0) {
      container.append(document.createTextNode(". "));
    }

    container.append(document.createTextNode(text));
  };

  const renderHonor = (honor) => {
    const item = document.createElement("li");
    const title = honor.title || honor.award || "Honor";

    if (honor.year) {
      const year = document.createElement("span");
      year.className = "award-year";
      year.textContent = honor.year;
      item.append(year);
    }

    const titleElement = document.createElement("strong");
    titleElement.textContent = title;
    item.append(titleElement);

    const details = document.createElement("span");
    appendSentencePart(details, honor.authors);
    appendSentencePart(details, honor.venue);

    if (honor.award) {
      if (details.childNodes.length > 0) {
        details.append(document.createTextNode(". "));
      }
      details.append(createLinkedText(honor.award, honor.award_url));
    }

    appendSentencePart(details, honor.notes);

    if (details.childNodes.length > 0) {
      item.append(details);
    }

    return item;
  };

  const loadHonors = async () => {
    if (!honorsUrl) {
      return;
    }

    try {
      const requestUrl = new URL(honorsUrl);
      requestUrl.searchParams.set("cacheBust", Date.now());
      const response = await fetch(requestUrl.toString());

      if (!response.ok) {
        throw new Error(`Honors CSV returned HTTP ${response.status}.`);
      }

      const honors = visibleSheetRows(parseCsv(await response.text()));

      if (honors.length === 0) {
        throw new Error("No visible honors were returned.");
      }

      honorsList.replaceChildren(...honors.map(renderHonor));
    } catch (error) {
      console.error("Unable to load honors from Google Sheet.", error);
    }
  };

  loadHonors();
}

const worksLists = Array.from(document.querySelectorAll("[data-works-list]"));

if (worksLists.length > 0) {
  const worksUrl = worksLists.find((list) => list.dataset.worksUrl)?.dataset.worksUrl || "";

  const createTableCell = (content, className = "") => {
    const cell = document.createElement("td");
    if (className) {
      cell.className = className;
    }

    if (content instanceof Node) {
      cell.append(content);
    } else {
      cell.textContent = content || "-";
    }

    return cell;
  };

  const renderWork = (work) => {
    const row = document.createElement("tr");
    if (work.year) {
      row.dataset.year = work.year;
    }

    const title = createLinkedText(work.title || "Untitled", work.title_url);
    const code = work.code_label || work.code_url ? createLinkedText(work.code_label || "Code", work.code_url) : "-";

    row.append(
      createTableCell(work.year, "pub-year-cell"),
      createTableCell(title),
      createTableCell(work.authors),
      createTableCell(work.venue),
      createTableCell(code),
    );

    return row;
  };

  const loadWorks = async () => {
    if (!worksUrl) {
      return;
    }

    try {
      const requestUrl = new URL(worksUrl);
      requestUrl.searchParams.set("cacheBust", Date.now());
      const response = await fetch(requestUrl.toString());

      if (!response.ok) {
        throw new Error(`Works CSV returned HTTP ${response.status}.`);
      }

      const works = visibleSheetRows(parseCsv(await response.text()));

      if (works.length === 0) {
        throw new Error("No visible works were returned.");
      }

      worksLists.forEach((list) => {
        const category = list.dataset.worksList || "";
        const rows = works.filter((work) => work.category === category);

        if (rows.length > 0) {
          list.replaceChildren(...rows.map(renderWork));
        }
      });
    } catch (error) {
      console.error("Unable to load works from Google Sheet.", error);
    }
  };

  loadWorks();
}

const studentsView = document.querySelector("[data-students-view]");

if (studentsView) {
  const apiUrl = studentsView.dataset.apiUrl || "";
  const studentsUrl = studentsView.dataset.studentsUrl || "";
  const statusElement = studentsView.querySelector("[data-students-status]");
  const listElement = studentsView.querySelector("[data-students-list]");

  const setStudentStatus = (message, isError = false) => {
    if (!statusElement) {
      return;
    }

    statusElement.textContent = message;
    statusElement.classList.toggle("is-error", isError);
    statusElement.hidden = false;
  };

  const createStudentLink = (student) => {
    const href = student.profile_url || "#";
    const link = document.createElement("a");
    link.href = href;

    const photoUrl = student.photo_url || student.photo_file || "";

    if (photoUrl) {
      const image = document.createElement("img");
      image.src = photoUrl;
      image.alt = "";
      image.loading = "lazy";
      link.append(image);
    }

    const name = document.createElement("span");
    name.textContent = student.name || "Unnamed student";
    link.append(name);

    if (student.title || student.research) {
      const meta = document.createElement("small");
      meta.textContent = student.title || student.research;
      link.append(meta);
    }

    return link;
  };

  const renderStudentGroup = (groupName, groupStudents) => {
    const section = document.createElement("section");
    section.className = "student-group";

    const heading = document.createElement("h3");
    heading.textContent = groupName;
    section.append(heading);

    const list = document.createElement("div");
    list.className = "student-directory-list";
    groupStudents.forEach((student) => list.append(createStudentLink(student)));
    section.append(list);

    return section;
  };

  const renderStudents = (students) => {
    const buckets = new Map([
      ["Current Students", new Map()],
      ["Alumni", new Map()],
    ]);

    visibleSheetRows(students).forEach((student) => {
      const graduated = String(student.graduated ?? "").trim().toLowerCase();
      const bucketName = ["true", "yes", "y", "1"].includes(graduated) ? "Alumni" : "Current Students";
      const bucket = buckets.get(bucketName);
      const group = student.group || "Students";

      if (!bucket.has(group)) {
        bucket.set(group, []);
      }

      bucket.get(group).push(student);
    });

    const totalStudents = Array.from(buckets.values()).reduce((total, groups) => {
      return total + Array.from(groups.values()).reduce((groupTotal, groupStudents) => groupTotal + groupStudents.length, 0);
    }, 0);

    if (totalStudents === 0) {
      throw new Error("No visible students were returned.");
    }

    const fragment = document.createDocumentFragment();

    buckets.forEach((groups, bucketName) => {
      if (groups.size === 0) {
        return;
      }

      const section = document.createElement("section");
      section.className = "student-status-group";

      const heading = document.createElement("h2");
      heading.textContent = bucketName;
      section.append(heading);

      groups.forEach((groupStudents, groupName) => {
        section.append(renderStudentGroup(groupName, groupStudents));
      });

      fragment.append(section);
    });

    listElement.replaceChildren(fragment);
  };

  const loadStudents = async () => {
    if ((!studentsUrl && !apiUrl) || !listElement) {
      return;
    }

    try {
      setStudentStatus("Loading student list...");
      let students = [];

      if (studentsUrl) {
        const requestUrl = new URL(studentsUrl);
        requestUrl.searchParams.set("cacheBust", Date.now());
        const response = await fetch(requestUrl.toString());

        if (!response.ok) {
          throw new Error(`Student CSV returned HTTP ${response.status}.`);
        }

        students = parseCsv(await response.text());

        if (!students.some((student) => Object.prototype.hasOwnProperty.call(student, "name"))) {
          throw new Error("Student CSV did not include the expected name column.");
        }
      } else {
        const payload = await loadAppsScriptData(apiUrl);

        if (!payload.ok) {
          throw new Error((payload.errors || []).join(" ") || "Student API returned an error.");
        }

        students = payload.students || [];
      }

      renderStudents(students);

      if (statusElement) {
        statusElement.hidden = true;
      }
    } catch (error) {
      setStudentStatus("無法讀取 Google Sheet 學生資料。請確認試算表已開放知道連結者可檢視，暫時顯示原本清單。", true);
      console.error(error);
    }
  };

  loadStudents();
}
