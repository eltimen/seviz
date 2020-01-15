let channel = new QWebChannel(qt.webChannelTransport,
    function (channel) {
        console.log("init");
        window.core = channel.objects.core;
        //window.core.setPageLoadedState(false);
    });

console.log(channel);

// TODO promise