//var debug_url = "http://192.168.0.11";
var debug_url = "";

var inti_url = debug_url + "/indexj";
var config_url = debug_url + "/config";
var edit_config_url = debug_url + "/editconfig";
var reset_config_url = debug_url + "/defaultsettings";
var restart_config_url = debug_url + "/restart";
var unlock_config_url = debug_url + "/stability";


var toUpdate = [];



function Initpage(){
    $.getJSON(inti_url, function (data) {
        $('#Parameters').html(Mustache.render($('#head-template').html(), data));
        $('#PlugIns').html(Mustache.render($('#plugin-template').html(), data));

        var parameters = data["parameters"]

        for (var i = 0; i < parameters.length; i++) {
            if (parameters[i].key === 'Stabile') {
              if(!parameters[i].value)
              {
                $("#unlock_button").show();
              }
            }
        }
    })
}


function UpdatePage() {
    toUpdate = [];
    
    $.getJSON(config_url, function (data) {
        $('#table_setting').html(Mustache.render($('#entry-template').html(), data));
    });
}

function valueChange(key){
    var value = $("#value_" + key);
    value.addClass("value_changed");

    var restoreButton = $("#restore_" + key);
    restoreButton.show();

    var present = toUpdate.indexOf(key);
    if(present < 0 ) toUpdate.push(key);
};


function restore(key) {
    var hidden = $("#hidden_" + key);
    var value = $("#value_" + key);
    var restoreButton = $("#restore_" + key);
    restoreButton.hide();

    value.val(hidden.val());
    value.removeClass("value_changed");

    var present = toUpdate.indexOf(key);
    toUpdate.splice(present, 1);
}

function Submit() {

    var parameters = "";

    if(toUpdate.length == 0)
    {
        $.alert({
            title: 'Attensione!',
            content: 'NESSUN PARAMETRO MODIFICATO! ',
            theme: 'supervan'
        });
        return; 
    }

    for(var i = 0 ; i < toUpdate.length ; i++)
    {
        var size = $("#size_" + toUpdate[i]).val();
        var value = $("#value_" + toUpdate[i]).val();
        if (value.length > size) {
            $.alert({
                title: 'Attensione!',
                content: 'Dimensione massima superata! per il parametro '  + toUpdate[i]
                +' <br> Dimensione massima: ' + size + ' <br> lunghezza: ' + value.length,
                theme: 'supervan'
            });
            return;
        }

        parameters += toUpdate[i] +", ";
    }

    

    $.confirm({
        title: 'Modifica parametri!',
        content: 'Sei sicuro di voler modificare i parametri:  '+parameters+' ? ',
        theme: 'supervan',
        buttons: {
            confirm: function () {
                $("#updateResult").val("");

                //$("#updateResult").val($("#updateResult").val() + );

                for(var i = 0 ; i < toUpdate.length ; i++)
                {
                    var key = toUpdate[i];
                    var value = $("#value_" + key).val();

                    value = value.replace(/&/g, "$"); 

                    $.ajax({
                        url: edit_config_url + "?name=" + key + "&val=" + value,
                        dataType: 'json',
                        async: false,
                        success: function(data) {
                            if (data.result == "W") 
                                $("#updateResult").val($("#updateResult").val() + "ATTENZIONE! PARAMETRO "+key+" INESITENTE <br>" );
                            if (data.result == "E")
                                $("#updateResult").val($("#updateResult").val() + "ATTENZIONE! ERRORE INTERNO DURANTE L'AGGIORNAMENTO DEL PARAMTERO "+key+". <br>");
                            if (data.result == "O")
                                $("#updateResult").val($("#updateResult").val() + "PARAMETRO "+key+" AGGIORNATO CORRETTAMENTE <br>");
                        }
                      });
                }

                $.alert({
                    title: 'AGGIORNAMENTO EFFETTUATO',
                    content: "L'AGGIORNAMENTO DEI PARAMETRI E' STATO EFFETTUATO CON I SEGUENTI RISULTATI <br> "+ $("#updateResult").val() ,
                    theme: 'supervan'
                });

                UpdatePage();
            },
            cancel: function () {
                return;
            },
        }
    });

    

}

function Reset() {
    $.confirm({
        title: 'RIPRISTINO CONFIGURAZIONI INIZIALI!',
        content: 'RESETTARE CONFIGURAZIONI? <br> TUTTE LE ATTUALI CONFIGURAZIONI SARANNO PERSE!',
        theme: 'supervan',
        buttons: {
            confirm: function () {
                $.confirm({
                    title: 'RIPRISTINO CONFIGURAZIONI INIZIALI!',
                    content: 'SI é SICURI DI VOLER RIPRISTINARE LE IMPOSTAZIONI DI FABBRICA? <br> TUTTE LE ATTUALI CONFIGURAZIONI SARANNO PERSE!',
                    theme: 'supervan',
                    buttons: {
                        confirm: function () {
                            $.getJSON(reset_config_url, function (data) {
                                if (data.result == "O") UpdatePage();
                            });
                        },
                        cancel: function () {
                        },
                    }
                });
            },
            cancel: function () {
            },
        }
    });
}


function Restart() {
    $.confirm({
        title: 'RIAVVIO DISPOSITIVO!',
        content: 'RIAVVIARE DISPOSITIVO?',
        theme: 'supervan',
        buttons: {
            confirm: function () {
                $.ajax({
                    url: restart_config_url,
                    dataType: 'json',
                    method: 'get',
                    timeout: 100
                }).done(function (response) {
                }).fail(function () {
                    $.confirm({
                        title: 'ATTENDERE IL RIAVVIO DEL DISPOSITIVO!',
                        theme: 'supervan',
                        content: function () {
                            var self = this;
                            return $.ajax({
                                url: config_url,
                                dataType: 'json',
                                method: 'get',
                                timeout: 20000
                            }).done(function (response) {
                                self.setContent('Premere ok per ricaricare la pagina');
                                self.setTitle('Riavvio Eseguito!!');
                            }).fail(function () {
                                self.setTitle('Something went wrong!!');
                                self.setContent(' Contattare l\'amministratore di sistema');
                            });
                        },
                        buttons: {
                            ok: function () {
                                UpdatePage();
                            },
                        }
                    });
                });
            },
            cancel: function () {
            },
        }
    });
}


function Unlock() {
    $.confirm({
        title: 'SBLOCCA E RIAVVIA IL DEVICE',
        content: 'SBLOCCA IL SISTEMA <br> ',
        theme: 'supervan',
        buttons: {
            confirm: function () {
                $.ajax({
                    url: unlock_config_url + "?val=0",
                    dataType: 'json',
                    async: false,
                    success: function(data) {
                        if (data.result == "W") 
                            $.alert({
                                title: 'ERRORE',
                                content: "PARAMETRO ERRATO",
                                theme: 'supervan'
                            });
                        if (data.result == "E")
                            $.alert({
                                title: 'ERRORE',
                                content: "ERRORE INTERNO DEL SERVER",
                                theme: 'supervan'
                            });
                        if (data.result == "O")
                        {
                            $.alert({
                                title: 'SBLOCCO EFFETTUATO',
                                content: "SISTEMA DEFINITO STABILE DALL' UTENTE <br> SI CONSIGLIA DI RIAVVIARE ",
                                theme: 'supervan'
                            }); 
                            Initpage(); 
                        }
                    }
                });
            },
            cancel: function () {
            },
        }
    });
}




$(function () {
    Initpage();
    UpdatePage();

    $("#refresh_button").on('click', UpdatePage);
    $("#reset_button").on('click', Reset);
    $("#restart_button").on('click', Restart);
    $("#submit_button").on('click', Submit);
    $("#unlock_button").on('click', Unlock);
    
    
});
