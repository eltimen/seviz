
class Render {
    book;
    rendition;
    prev;
    next; 

    constructor() {
        this.next = document.getElementById("next");
        this.next.addEventListener("click", function (e) {
            this.rendition.next();
            e.preventDefault();
        }.bind(this), false);

        this.prev = document.getElementById("prev");
        this.prev.addEventListener("click", function (e) {
            this.rendition.prev();
            e.preventDefault();
        }.bind(this), false);
    };

    open(path) {
        console.log(path);
        this.book = ePub(path);
        this.rendition = this.book.renderTo("viewer", {
            flow: "scrolled-doc",
            width: "100%"
        });
        this._setup();
        this.rendition.display();
    };

    _setup() {
        this.rendition.on("relocated", function (location) {
            console.log(location);
        }.bind(this));

        this.rendition.on("rendered", function (section) {
            var nextSection = section.next();
            var prevSection = section.prev();

            if (nextSection) {
                let nextNav = this.book.navigation.get(nextSection.href);

                let nextLabel;
                if (nextNav) {
                    nextLabel = nextNav.label;
                } else {
                    nextLabel = "next";
                }

                this.next.textContent = nextLabel + " >>";
            } else {
                this.next.textContent = "";
            }

            if (prevSection) {
                let prevNav = this.book.navigation.get(prevSection.href);

                let prevLabel;
                if (prevNav) {
                    prevLabel = prevNav.label;
                } else {
                    prevLabel = "previous";
                }

                this.prev.textContent = "<< " + prevLabel;
            } else {
                this.prev.textContent = "";
            }

        }.bind(this)); 
    }
}

let render = new Render();
window.render = render;