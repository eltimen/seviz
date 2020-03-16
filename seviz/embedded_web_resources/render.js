function addListenerMulti(el, s, fn) {
    s.split(' ').forEach(e => el.addEventListener(e, fn, false));
} 

function clearStyles(el) {
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
    // TODO кешировать результат для каждой главы
    // TODO реакция на некорректный символ в предложении. необходимо отбросить его и продолжить работу
    let outParagraphs = [];
    pars = viewer.getElementsByTagName("p");
    for (let i = 0; i < pars.length; ++i) {
        /*
            "paragraphs": [ { "id": 1, "sentences": [ { "id": 1, "text": [{1,"word1"}, {2,"."}] } ] } ] }
        */

        pars[i].setAttribute("id", i + 1);         
        let rawSplitData = splitter.split(pars[i].textContent);
        let parInnerHtml = "";
        let sentArr = [];
        // собираем абзац как набор маркированных предложений
        let sentId = 1;
        rawSplitData.forEach(el => {
            if (el.type == "Sentence") {
                let wordsArr = [];
                let sentHtml = "<sentence id=\"" + String(sentId) + "\">";
                let words = el.raw.match(/([\w]+|\.|,|"|'|:|”|“|!|\(|\)|;|‘|’)/g);
                for (let wordId = 0; wordId < words.length; ++wordId) {
                    wordsArr.push({ id: wordId + 1, text: words[wordId] });
                    sentHtml += "<word id=\"" + String(wordId + 1) + "\">" + words[wordId] + " </word>";
                }
                sentHtml += "</sentence> ";
                sentArr.push({ id: sentId, words: wordsArr });
                parInnerHtml += sentHtml;
                sentId++;
            } 
        });
        outParagraphs.push({ id: i + 1, sentences: sentArr });
        pars[i].innerHTML = parInnerHtml;
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
        this.alreadyOpened = true;

        this.book.loaded.navigation.then(function (toc) {

            let processSubItems = (chapter) => {
                chapter.subitems.forEach((el) => {
                    this.chapters.push(el);
                    processSubItems(el);
                });
            };

            toc.forEach(function (chapter) {
                this.chapters.push(chapter);
                processSubItems(chapter);

            }.bind(this));

            //$select.appendChild(docfrag);

            //$select.onchange = function () {
            //    var index = $select.selectedIndex,
            //        url = $select.options[index].ref;
            //    display(url);
            //    return false;
            //};

            this.book.opened.then(function () {
                window.core.setChaptersList(this.chapters);
            }.bind(this));

        }.bind(this));
        
    };

    // показывает элемент chapters по заданному индексу
    display(i) {
        console.log(i);
        // получаем html файл с нужной главой
        var section = this.book.spine.get(this.chapters[i].href);
        if (section) {
            section.render().then(function (html) {
                // собираем dom-дерево, содержащее только заданную главу
                this.viewer.innerHTML = "";
                let chapterDoc = new DOMParser().parseFromString(html, 'text/html');
                // находим id начала этой главы и следующей.
                // TODO поменять способ итерации на учитывающий дочерние узлы
                // TODO реализовать загрузку данной главы из соседних html файлов. сейчас показывается только из начального html
                let from = this.chapters[i].href.split('#')[1];
                let to = i+1 < this.chapters.length ? this.chapters[i+1].href.split('#')[1] : null;
                console.log('from: ' + from + ' to: ' + to);

                // проверяем, что ссылка на главу не содержит в себе селектор элемента из html-файла секции
                if (from == null && to == null) {
                    this.viewer.innerHTML = html;
                } else {
                    // иначе вырезаем главу из остальных элементов страницы
                    let currentElem = chapterDoc.getElementById(from);
                    while (currentElem != null && (currentElem.id != to || to == null)) {
                        // показываем элемент
                        this.viewer.appendChild(currentElem.cloneNode(true));
                        currentElem = currentElem.nextSibling;
                        if (currentElem == null) console.log('currentElem = null'); // debug
                    }
                }

                // блокируем все ссылки
                let lnks = this.viewer.getElementsByTagName("a");
                for (let i = 0; i < lnks.length; i++) {
                    lnks[i].onclick = function () { return false; };
                }

                // инициализируем модель
                let model = markParagraphs(this.viewer);
                window.core.setModelDataForChapter(i, model);

                setupHandlers(this.viewer);
            }.bind(this));
        }

        return section;
    }

    close() {
        if (this.book != undefined) {
            //this.rendition.destroy();
        }
    }

}

let render = new Render();
window.render = render;