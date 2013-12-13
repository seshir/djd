(function ($) {
    Drupal.behaviors.swisscomSmstoken = {
        attach: function (context) {
            $('.form-submit').once().ajaxStart(function() {
                $('.alert').remove();
            });
        }
    };
})(jQuery);