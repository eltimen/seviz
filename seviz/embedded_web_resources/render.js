// Load the opf
var book = ePub("ivanhoe.epub");
var rendition = book.renderTo("viewer", {
    flow: "scrolled-doc",
    width: "100%"
});

rendition.display();

var next = document.getElementById("next");
next.addEventListener("click", function (e) {
    rendition.next();
    e.preventDefault();
}, false);

var prev = document.getElementById("prev");
prev.addEventListener("click", function (e) {
    rendition.prev();
    e.preventDefault();
}, false);

rendition.on("relocated", function (location) {
    console.log(location);
});

rendition.on("rendered", function (section) {
    var nextSection = section.next();
    var prevSection = section.prev();

    if (nextSection) {
        nextNav = book.navigation.get(nextSection.href);

        if (nextNav) {
            nextLabel = nextNav.label;
        } else {
            nextLabel = "next";
        }

        next.textContent = nextLabel + " »";
    } else {
        next.textContent = "";
    }

    if (prevSection) {
        prevNav = book.navigation.get(prevSection.href);

        if (prevNav) {
            prevLabel = prevNav.label;
        } else {
            prevLabel = "previous";
        }

        prev.textContent = "« " + prevLabel;
    } else {
        prev.textContent = "";
    }

});