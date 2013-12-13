(function ($) {
  "use strict";

  Drupal.behaviors.itoggleNodeFieldsetSummaries = {
    attach: function (context) {
      $('fieldset.node-form-options', context).drupalSetSummary(function (context) {
        var vals = [];

        $('div.form-type-itoggle', context).each(function () {
          vals.push(Drupal.checkPlain($.trim($(this).text())));
        });

        if (!$('.form-item-status input', context).is(':checked')) {
          vals.unshift(Drupal.t('Not published'));
        }
        return vals.join(', ');
      });
    }
  };

})(jQuery);