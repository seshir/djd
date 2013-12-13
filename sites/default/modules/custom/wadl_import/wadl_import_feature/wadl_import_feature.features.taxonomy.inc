<?php
/**
 * @file
 * wadl_import_feature.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function wadl_import_feature_taxonomy_default_vocabularies() {
  return array(
    'api_name' => array(
      'name' => 'API Name',
      'machine_name' => 'api_name',
      'description' => 'The high level API Name for the bundle of API Methods and API Entity Groups.',
      'hierarchy' => '1',
      'module' => 'wadl_import',
      'weight' => '-10',
      'metatags' => array(),
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'api_tag' => array(
      'name' => 'API Tag',
      'machine_name' => 'api_tag',
      'description' => 'The entity group is derived from the apigee:tag from the WADL.',
      'hierarchy' => '1',
      'module' => 'wadl_import',
      'weight' => '-9',
      'metatags' => array(),
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'tags' => array(
      'name' => 'Tags',
      'machine_name' => 'tags',
      'description' => 'Use tags to group articles on similar topics into categories.',
      'hierarchy' => '0',
      'module' => 'taxonomy',
      'weight' => '0',
      'metatags' => array(),
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}
