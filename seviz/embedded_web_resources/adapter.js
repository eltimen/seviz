let channel = new QWebChannel(qt.webChannelTransport,
    function (channel) {
        console.log("init");
    });

console.log(channel);