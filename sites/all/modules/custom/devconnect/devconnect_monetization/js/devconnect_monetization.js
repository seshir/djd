(function($) {
	Drupal.behaviors.devconnect_monetization = {
		attach: function(context) {
			$('select#api_product').selectList({
			  instance: true,
			  clickRemove: false,
			  onAdd: function (select, value, text) {
				$("#edit-submit").attr("disabled", "disabled");
				$.ajax({
				  url: "/users/me/monetization/accepted-product/" + encodeURI(value),
				  accept: 'application/json; utf-8',
				  async: true,
				  cache: false,
				  success: function(data) {
					if (data.found == false) {
					  $('.selectlist-item').each(function(index, item){
						  if ($(item).html().trim() == text) {
							  $(item).remove();
						  }
					  });
					  $("p", "#dialog-modal").html(data.message);
					  $( "#dialog-modal" ).dialog({
					    height: 160,
					    width: 450,
					    modal: true,
					    buttons: {
					      "Continue": function() {
					    	$( this ).dialog( "close" );
					      }
				    	}
					  });
					}
					else {
						$('.selectlist-item').last().append('<span class="delete"></span>');
					}
				  },
				  complete: function() {
					  $("#edit-submit").removeAttr("disabled");
				  }
				});
			  }
			});
			$("#previous_prepaid_stmt_download").click(function(e) {
				if ($("#previous_prepaid_stmt_download").attr("href").length = 1) {
					e.stopPropagation();
					e.preventDefault();
					alert("Select an account and a month");
				}
			});
		},
		detach: function(context) {
		}
	};
})(jQuery);