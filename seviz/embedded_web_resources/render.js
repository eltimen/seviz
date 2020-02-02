
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
                this.display(0);
                window.core.setChaptersList(this.chapters);
            }.bind(this));

        }.bind(this));
        
    };

    // показывает элемент chapters по заданному индексу
    display(i) {
        // получаем html файл с нужной главой
        var section = this.book.spine.get(this.chapters[i].href);
        if (section) {
            section.render().then(function (html) {
                // собираем dom-дерево, содержащее только заданную главу
                this.viewer.innerHTML = "";
                let chapterDoc = new DOMParser().parseFromString(html, 'text/html');
                // находим id начала этой главы и следующей.
                // TODO реализовать загрузку данной главы из соседних html файлов. сейчас показывается только из начального html
                let from = this.chapters[i].href.split('#')[1];
                let to = i+1 < this.chapters.length ? this.chapters[i+1].href.split('#')[1] : null;
                console.log('from: ' + from + ' to: ' + to);
      
                let currentElem = chapterDoc.getElementById(from);
                while (currentElem != null && (currentElem.id != to || to == null)) {
                    this.viewer.appendChild(currentElem.cloneNode(true));
                    currentElem = currentElem.nextSibling;
                }

                //this.viewer.innerHTML = html;
            }.bind(this));
        }

        return section;
    }

    close() {
        if (this.book != undefined) {
            this.rendition.destroy();
        }
    }

}

let render = new Render();
window.render = render;