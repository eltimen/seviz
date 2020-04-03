function addListenerMulti(el, s, fn) {
    s.split(' ').forEach(e => el.addEventListener(e, fn, false));
} 

let tooltips = [];

function addTooltip(selector, text) {
    let el = document.querySelector(selector);
    el.setAttribute("data-tooltip", text);
    tooltips.push(el)
}

function cleanupBeforeRender(el) {
    let pars = el.getElementsByTagName('p');
    for (let p of pars) {
        for (let s of p.children) {
            for (let w of s.children) {
                w.style.cssText = "";
            }
            s.style.cssText = "";
        }
        p.style.cssText = "";
    }

    document.querySelectorAll("SPAN.supsub > *").forEach(el => {
        el.style.cssText = "";
        el.textContent = "";
    });

    tooltips.forEach(el => {
        el.removeAttribute("data-tooltip");
    });
    tooltips = [];
}

function makePos(node) {
    let pos = {};
    if (node.tagName == "WORD") {
        pos.word = Number(node.id);
        pos.sentence = Number(node.parentElement.id);
        pos.paragraph = Number(node.parentElement.parentElement.id);
    } else if (node.tagName == "SENTENCE") {
        pos.word = -1;
        pos.sentence = Number(node.id);
        pos.paragraph = Number(node.parentElement.id);
    } else if (node.tagName == "P") {
        pos.word = -1;
        pos.sentence = -1;
        pos.paragraph = Number(node.id);
    } 
    
    return pos;
}

function mouseHoverElement() {
    let hoverElements = document.querySelectorAll(":hover");
    let last = hoverElements[hoverElements.length - 1];
    if (last) {
        return makePos(last);
    }
    return {};
}

function getSelectedElements(allowPartialSelection) {
    let selection = getSelection();
    let allSelected = [];

    if (selection.type == 'Range') {
        let container = selection.getRangeAt(0).commonAncestorContainer;

        let allInsideParent;
        if (container.nodeName == "#text") {
            allInsideParent = [container.parentElement];
        } else {
            allInsideParent = container.getElementsByTagName('word');
        }

        for (let i = 0, el; el = allInsideParent[i]; ++i) {
            // true - allow partial contain
            if (selection.containsNode(el, allowPartialSelection)) {
                //if (el.tagName == "WORD" && el.parentElement.tagName == "SENTENCE" && el.parentElement.parentElement.tagName == "P") {
                allSelected.push(el);
                //}
            }
        }
    }

    return allSelected;
}

function getSelectionBorders() {
    // TODO реакция, когда выделено, например, название главы - это не текст. сейчас в этом случае берет самый первый элемент word
    let selected = getSelectedElements(true);
    if (selected.length > 0) {
        return [makePos(selected[0]), makePos(selected[selected.length - 1])];
    }
    return [];
}

function setupHandlers(viewer) {
    var elements = viewer.getElementsByTagName('p');
    for (var i = 0, len = elements.length; i < len; i++) {
        addListenerMulti(elements[i], 'auxclick click contextmenu dblclick mousedown mousemove mouseover mouseout mouseup', (event) => {
            //console.log(event);
            core.processEvent(JSON.stringify(event, ["type", "altKey", "ctrlKey", "shiftKey", "path", "id", "tagName"]));
        });
    }
}

function markParagraphs(viewer) {
    // TODO реакция на некорректный символ в предложении. необходимо отбросить его и продолжить работу
    let outParagraphs = [];
    pars = viewer.getElementsByTagName("p");
    for (let i = 0; i < pars.length; ++i) {
        /*
            "paragraphs": [ { "id": 1, "sentences": [ { "id": 1, "text": [{1,"word1"}, {2,"."}] } ] } ] }
        */

        var strParId = String(i + 1);
        let par = pars[i];
        let parParent = par.parentNode;

        let beforeNode =  document.createElement('span');
        var domString = "<span class=\"supsub\"><sup id=\"shl" + strParId + "\"></sup><sub id=\"sdl" + strParId + "\"></sub></span>";
        beforeNode.innerHTML = domString;
        parParent.insertBefore(beforeNode.firstChild, par); 

        let afterNode = document.createElement('span');
        var domString = "<span class=\"supsub\"><sup id=\"shr" + strParId + "\"></sup><sub id=\"sdr" + strParId + "\"></sub></span>";
        afterNode.innerHTML = domString;
        parParent.insertBefore(afterNode.firstChild, par.nextSibling);

        par.setAttribute("id", i + 1);         
        let rawSplitData = splitter.split(par.textContent);
        let parInnerHtml = "";
        let sentArr = [];
        // собираем абзац как набор маркированных предложений
        let sentId = 1;
        rawSplitData.forEach(el => {
            if (el.type == "Sentence") {
                let wordsArr = [];
                let strSentTailId = strParId + "_" + String(sentId);
                let sentHtml = "<span class=\"supsub\"><sup id=\"shl" + strSentTailId + "\"></sup><sub id=\"sdl" + strSentTailId + "\"></sub></span>" +
                               "<sentence id=\"" + String(sentId) + "\">";
                let words = el.raw.match(/([\w]+|\.|,|"|'|:|”|“|!|\(|\)|;|‘|’)/g);
                for (let wordId = 0; wordId < words.length; ++wordId) {
                    wordsArr.push({ id: wordId + 1, text: words[wordId] });
                    let strWordId = String(wordId + 1);
                    let strTailId = strParId + "_" + String(sentId) + "_" + strWordId;
                    sentHtml += "<span class=\"supsub\"><sup id=\"shl" + strTailId + "\"></sup><sub id=\"sdl" + strTailId + "\"></sub></span>" +
                                "<word id=\"" + strWordId + "\">" + words[wordId] + " </word>" +
                                "<span class=\"supsub\"><sup id=\"shr" + strTailId + "\"></sup><sub id=\"sdr" + strTailId + "\"></sub></span>";
                }
                sentHtml += "</sentence>" +
                            "<span class=\"supsub\"><sup id=\"shr" + strSentTailId + "\"></sup><sub id=\"sdr" + strSentTailId + "\"></sub></span>";
                sentArr.push({ id: sentId, words: wordsArr });
                parInnerHtml += sentHtml;
                sentId++;
            } 
        });
        outParagraphs.push({ id: i + 1, sentences: sentArr });
        par.innerHTML = parInnerHtml;
    }
    return outParagraphs;
};

class Render {
    book;
    alreadyOpened;
    viewer;
    chapters;

    constructor() {
        this.alreadyOpened = false;
        this.viewer = document.getElementById("viewer");
    };

    open(path) {
        console.log(path);
        document.getElementById("help").style.display = "none";
        document.getElementById("viewer").style.visibility = "visible";
        this.book = ePub(path);
        this.chapters = [];
        this.toc = {};
        this.alreadyOpened = true;

        this.book.loaded.navigation.then(function (toc) {

            let processSubItems = (chapter, depth) => {
                if (depth === 0) {
                    return;
                }
                chapter.subitems.forEach((el) => {
                    this.chapters.push(el);
                    processSubItems(el, depth-1);
                });
            };

            this.toc = toc;

            console.log(toc);

            toc.forEach(function (chapter) {
                this.chapters.push(chapter);
                processSubItems(chapter, 1);

            }.bind(this));

            this.book.opened.then(function () {
                let chaptersCount = this.chapters.size;
                this.chapterData = Array.apply(null, Array(chaptersCount)).map(function () { });
                window.core.setChaptersList(this.chapters);
            }.bind(this));

        }.bind(this));
        
    };

    // извлекает в this.result html куска главы из файла. если глава содержится в нескольких файлах, рекурсивно вызывает сама себя, пока конец главы не найден
    // после того, как вся глава будет на экране, вызывает callback
    result;
    extractChapterPartHtml = function (i, foundEndOfChapter, callback, searchTo) {
        console.log('call ', i, foundEndOfChapter, searchTo);
        if (!foundEndOfChapter) {
            // получаем html файл (spine) с нужной главой. если глава разбита на несколько файлов, рекурсивно загружаем соседние
            var section = this.book.spine.get(this.chapters[i].href);
            if (section) {
                section.render().then(function (html) {
                    // собираем dom-дерево, содержащее только заданную главу
                    let chapterDoc = new DOMParser().parseFromString(html, 'text/html');
                    // находим id начала этой главы и следующей.
                    let idFrom = this.chapters[i].href.split('#')[1];
                    let idTo = i + 1 < this.chapters.length ? this.chapters[i + 1].href.split('#')[1] : null;

                    // если url файла не содержит никаких ID, то глава вся находится в файле
                    if (idFrom == null && idTo == null) {
                        this.result.innerHTML += html;
                        console.log("inner");
                    } else {
                        // иначе вырезаем главу из остальных элементов страницы
                        let currentElem;
                        if (searchTo != undefined) {
                            currentElem = chapterDoc.body.firstElementChild;
                            idTo = searchTo;
                        } else {
                            currentElem = chapterDoc.getElementById(idFrom);
                        }
                        console.log('idFrom: ' + idFrom + ' idTo: ' + idTo);
                        while (currentElem != null && (currentElem.id != idTo || idTo == null)) {
                            // показываем элемент
                            this.result.appendChild(currentElem.cloneNode(true));
                            // TODO не просто проверить, есть ли в следующем узле элемент оглавления idTo, но и вырезать все элементы до него
                            if (currentElem instanceof Element && currentElem.querySelector("#" + idTo) != null) {
                                break;
                            }
                            currentElem = currentElem.nextSibling;
                            // если достигнут конец текущего уровня иерархии DOM html-файла, но не найден конец главы
                            if (idTo != null && currentElem == null) {
                                console.log('call 2');
                                return this.extractChapterPartHtml(i + 1, false, callback, idTo);
                            } 
                        }
                        return this.extractChapterPartHtml(i, true, callback);
                    }
                }.bind(this));
            }
        } else {
            console.log('callback');
            callback(this.result);
        }
    }.bind(this);

    // показывает элемент chapters по заданному индексу
    display(i) {
        if (this.chapterData[i] == undefined) {
            console.log("making");
            this.result = document.createElement("div");
            this.extractChapterPartHtml(i, false, function () {
                // блокируем все ссылки
                let lnks = this.result.getElementsByTagName("a");
                for (let i = 0; i < lnks.length; i++) {
                    lnks[i].onclick = function () { return false; };
                }

                // инициализируем модель
                let model = markParagraphs(this.result);
                window.core.setModelDataForChapter(i, model);

                this.chapterData[i] = this.result;

                this.viewer.innerHTML = this.chapterData[i].innerHTML;
                setupHandlers(this.viewer);
            }.bind(this));
        } else {
            console.log("exist");
            this.viewer.innerHTML = this.chapterData[i].innerHTML;
            setupHandlers(this.viewer);
        }
    }

    close() {
        if (this.book != undefined) {
            //this.rendition.destroy();
        }
    }

}

let render = new Render();
window.render = render;