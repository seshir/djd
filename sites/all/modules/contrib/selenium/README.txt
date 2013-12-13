README.txt
==========

WHAT DOES SELENIUM MODULE DO?
=============================

	This module integrates Drupal with the Selenium Webdriver.
	This makes it possible to test every aspect of a Drupal website as Selenium
communicates with browser directly. Tests are written in simpletest tests manner.

Features:

		* Selenium tests communicate with simpletest sandbox
		* Complete integration with simpletest to run tests and see results
    * Can be run on headless server. See http://ygerasimov.com/run-selenium-tests-drupal-on-debian-headless

What is Selenium?

	Selenium is a testing tool for web applications. History of Selenium can be found
here http://seleniumhq.org/docs/01_introducing_selenium.html.

With Selenium we can:

    * Open pages and assert different elements (for example title of the page, headings etc)
    * Submit forms including all kinds of AJAX forms
    * Find elements on the page using XPath, CSS selectors and others
    * Upload files to the forms
    * Work with dialog windows, iframes, different popup windows etc.
    * Drag and drop elements on the page
    * Execute custom javascript on the page

EXAMPLES
========

You can find some examples in the tests folder.

BROWSERS
========

By default tests run in Firefox. If you would like to run them in Chrome, in
your setUp() implementation pass 'chrome' as first argument to parent::setUp().


PITFALLS
========

Please be aware that Selenium can't work with hidden elements on the page. The
logic behind this as Selenium reproduce manual testing where of course user can't
for example set value of the hidden textfield.

So please remember to unhide elements you are going to interact. Example with
vertical tabs module can be found in disableJs method.

Chrome cannot upload files.

Chrome cannot make screenshots.

If you would like to select one of the selectbox options use click() method
instead of select() as select() does not work in Chrome.
