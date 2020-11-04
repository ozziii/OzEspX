var config_url = "/config";

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

function _(el) {
    return document.getElementById(el);
}

function uploadFile() {
    var file = _("file1").files[0];

    if(!file)
    {
        $.confirm({
            title: 'No File Selected!',
            theme: 'supervan',
            content: 'Select a file',
            buttons: { Ok: { text: 'Ok' }}
        });
        return
    } 

    var formdata = new FormData();
    formdata.append("file1", file);
    var ajax = new XMLHttpRequest();
    ajax.upload.addEventListener("progress", progressHandler, false);
    ajax.addEventListener("load", completeHandler, false);
    ajax.addEventListener("error", errorHandler, false);
    ajax.addEventListener("abort", abortHandler, false);
    ajax.open("POST", "update");
    ajax.send(formdata);
}

function progressHandler(event) {
    _("loaded_n_total").innerHTML = "Uploaded " + event.loaded + " bytes of " + event.total;
    var percent = (event.loaded / event.total) * 100;
    _("progressBar").value = Math.round(percent);
    _("status").innerHTML = Math.round(percent) + "% uploaded... please wait";
}

function completeHandler(event) {
    var result = event.target.responseText;
    if (result == "O") {
        $.confirm({
            title: 'ATTENDERE IL RIAVVIO DEL DISPOSITIVO!',
            theme: 'supervan',
            content: '<div class="lds-ring"><div></div><div></div><div></div><div></div></div>',
            buttons: {
                Ok: {
                    isHidden: true,
                    action: function () {
                        document.location.href = "/";
                    }
                }
            },
            onContentReady: function () {
                var self = this;
                sleep(2000).then(() => {
                    return $.ajax({
                        url: config_url,
                        dataType: 'json',
                        method: 'get',
                        timeout: 20000
                    }).done(function (response) {
                        self.setContent('Premere ok per tornare alla Home');
                        self.setTitle('Riavvio Eseguito!!');
                        self.buttons.Ok.show();
                    }).fail(function () {
                        self.setTitle('Something went wrong!!');
                        self.setContent(' Contattare l\'amministratore di sistema');
                        self.buttons.Ok.show();
                    });
                });
            }
        });
    }

    if (result == "E") {
        _("status").innerHTML = "Update ERROR ";
    }


    _("progressBar").value = 0;
}

function errorHandler(event) {
    _("status").innerHTML = "Upload Failed";
}

function abortHandler(event) {
    _("status").innerHTML = "Upload Aborted";
}