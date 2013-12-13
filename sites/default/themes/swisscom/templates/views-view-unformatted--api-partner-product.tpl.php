<div class="biz-api-overview">
		<?php foreach ($rows as $id => $row): ?>
		  <div<?php if ($classes_array[$id]) { print ' class="' . $classes_array[$id] .'"';  } ?>>
			<?php print $row; ?>
		  </div>
		<?php endforeach; ?>
	<p>&nbsp;</p>
</div>