(function ($) {
		var currentFrame = -1;
		var frameCount = 88;
		var frameWidth = 48;
		var loop = 2;
		var logo;
		var intervalHandler;
		var staticLogoSrc;
		//speed in frame/ms
		var speed = 1 / 100;
		var clamp = 20;
		var effect = 3;

		var currentSpeed = 0;

		var previousPosition = null;
		var previousTime = null;
		var updateSpeed = function(event) {
			var newPosition = {x: event.pageX, y: event.pageY};
			var newTime = +new Date;
			if (newTime === previousTime) {
				previousPosition = newPosition;
				return;
			}
			if (previousPosition && previousTime) {
				var distance = Math.sqrt(Math.pow(previousPosition.x - newPosition.x, 2), Math.pow(previousPosition.y - newPosition.y));
				var timeDifference = newTime - previousTime;
				currentSpeed = distance / timeDifference;
			}
			previousTime = newTime;
			previousPosition = newPosition;
		};

		var animate = function() {
			currentFrame = (currentFrame + 1) % frameCount;
			if (currentFrame === 0) {
				loop--;
				window.clearInterval(intervalHandler);
				intervalHandler = null;
				if (loop === 0) {
					logo.attr('src', staticLogoSrc).css('marginLeft', 0);
					$(document).unbind('mousemove', updateSpeed);
					return;
				}
				else {
					speed = 1 / 55;
				}
			}
			logo.css('marginLeft', '-' + (currentFrame * frameWidth) + 'px');
			var interval = speed * (1 + (currentSpeed * effect));
			interval = Math.max(speed / clamp, Math.min(speed * clamp, interval));
			interval = 1 / interval;
			intervalHandler = window.setTimeout(arguments.callee, interval);
		};

		$(window).bind('load', function() {
			logo = $('#logo-swisscom-image');
			staticLogoSrc = logo.attr('src');
			if (!staticLogoSrc) {
				return;
			}
			var spriteLogoSrc = staticLogoSrc.replace(/\.png$/, '-sprites.png');

			var activate = function() {
				if ($.browser.msie && $.browser.version.split('.')[0] < 8) {
					return;
				}
				logo.attr('src', spriteLogoSrc);
				currentFrame = 0;
				$(document).bind('mousemove', updateSpeed);
				intervalHandler = window.setTimeout(animate, 1 / speed);
			};

			var preloader = $('<img/>', {src: spriteLogoSrc});
			if (preloader.prop('complete')) {
				activate();
			}
			else {
				preloader.bind('load', activate);
			}
		});
})(jQuery);