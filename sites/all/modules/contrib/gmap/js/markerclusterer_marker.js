
/**
 * @file
 * GMap Markers
 * Gmaps Utility Library MarkerClusterer API version
 */

/*global Drupal, GMarker, MarkerClusterer */

// Replace to override marker creation
Drupal.gmap.factory.marker = function (loc, opts) {
  return new GMarker(loc, opts);
};

Drupal.gmap.addHandler('gmap', function (elem) {
  var obj = this;

  obj.bind('init', function () {
    // Set up the markermanager.
    // Make sure the gridSize and maxZoom are intergers.
    if (Drupal.settings.gmap_markermanager.gridSize){
      Drupal.settings.gmap_markermanager.gridSize = parseInt(Drupal.settings.gmap_markermanager.gridSize);
    }
    if (Drupal.settings.gmap_markermanager.maxZoom){
      Drupal.settings.gmap_markermanager.maxZoom = parseInt(Drupal.settings.gmap_markermanager.maxZoom);
    }
    obj.mc = new MarkerClusterer(obj.map, [], Drupal.settings.gmap_markermanager);
  });
  obj.bind('addmarker', function (marker) {
    // @@@ Would be really nice to have bulk adding support in gmap.
    obj.mc.addMarkers([marker.marker]);
  });

  obj.bind('delmarker', function (marker) {
    obj.mc.removeMarker(marker.marker);
  });

  obj.bind('clearmarkers', function () {
    obj.mc.clearMarkers();
  });
});
