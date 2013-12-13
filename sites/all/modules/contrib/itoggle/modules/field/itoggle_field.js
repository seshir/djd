(function ($) {
  "use strict";

  Drupal.behaviors.itoggle_field = {
    attach: function(context, settings) {
      var $display_type = $('select#edit-instance-widget-settings-itoggle-settings-display-type', context);

      if ($display_type.length) {
        $display_type.bind('change blur', function() {
          switch (parseInt($(this).val(), 10)) {
            case 0:
              $('div.itoggle-wrapper', context).removeClass('itoggle-display-yesno itoggle-display-10').addClass('itoggle-display-onoff');
              break;
            case 1:
              $('div.itoggle-wrapper', context).removeClass('itoggle-display-onoff itoggle-display-10').addClass('itoggle-display-yesno');
              break;
            case 2:
              $('div.itoggle-wrapper', context).removeClass('itoggle-display-yesno itoggle-display-onoff').addClass('itoggle-display-10');
              break;
          }
        });
      }
    }
  };

})(jQuery);