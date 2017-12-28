(function($){
  'use strict';

  $(document).ready(function() {
    $('.btn-generate-sitemap').click(function() {
      $('#edit-script').val('_user_regenerate_sitemap_link($entity, ' + $(this).attr('data-status') + ');');
    });
  });

})(jQuery);
