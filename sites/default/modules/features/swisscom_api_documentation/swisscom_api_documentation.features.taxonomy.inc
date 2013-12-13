<?php
/**
 * @file
 * swisscom_api_documentation.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function swisscom_api_documentation_taxonomy_default_vocabularies() {
  return array(
    'api_name' => array(
      'name' => 'API Name',
      'machine_name' => 'api_name',
      'description' => 'The high level API Name for the bundle of API Methods and API Entity Groups.',
      'hierarchy' => '0',
      'module' => 'wadl_import',
      'weight' => '-10',
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
      'hierarchy' => '0',
      'module' => 'wadl_import',
      'weight' => '-9',
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
