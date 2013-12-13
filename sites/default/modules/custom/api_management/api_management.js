(function($) {

	Drupal.behaviors.api_management = {
		charts: [],
		attach: function(context, settings) {
			if ($("#api-initialize-remove").length > 0) {
				// Don't execute on Views Admin Page
				if ($('.views-admin').length == 0) {
					$('#api-initialize-remove').trigger('click');
					$('#api-initialize-remove').css('display', 'none');
				}
			}
		},
		detach: function(context) {
		}
	}
})(jQuery);
