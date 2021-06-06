<!-- 
  Code based on
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
-->
<!DOCTYPE html>
<html>
<head>
   <title>EspCamSend Gallery</title>
   <meta name="viewport" content="width=device-width, initial-scale=1.0">
   <style>
      .flex-container {
         display: flex;
         flex-wrap: wrap;
      }
      .flex-container > div {
         text-align: center;
         margin: 10px;
      }
   </style>
</head>
<body>
   <h2>EspCamSend Gallery</h2>
   <?php
      // Image extensions
      $image_extensions = array("png","jpg","jpeg","gif");

      // Check delete HTTP GET request - remove images
      if (isset($_GET["delete"])) {
         $imageFileType = strtolower(pathinfo($_GET["delete"],PATHINFO_EXTENSION));
         if (file_exists($_GET["delete"]) && ($imageFileType == "jpg" || $imageFileType == "png" || $imageFileType == "jpeg"|| $imageFileType == "txt") ) {
            echo "File found and deleted: " .  $_GET["delete"];
            unlink($_GET["delete"]);
         } else {
            echo 'File not found - <a href="gallery.php">refresh</a>';
         }
      }

      // Target directory
      $dir = 'uploads/';
      if (is_dir($dir)) {
         echo '<div class="flex-container">';
         $count = 1;
         $files = scandir($dir);
         rsort($files);
         foreach ($files as $file) {
            $info  = pathinfo($file);
            if ($file != '.' && $file != '..') {?>
               <div>
                  <p>
                     <a href="gallery.php?delete=<?php echo $dir . $file; ?>">Delete file</a> - <?php echo $file; ?>
                  </p>
                  <?php if ($info["extension"] == "txt") {?>
                  <p>
                     <object width="350" height="200" type="text/plain" data="<?php echo $dir . $file; ?>" border="0" style="overflow: hidden;"></object>
                  </p>
                  <?php } ?>
                  <a href="<?php echo $dir . $file; ?>">
                     <img src="<?php echo $dir . $file; ?>" style="width: 350px;" alt="" title=""/>
                  </a>
               </div>
           <?php $count++;
        }
     }
  }
  if($count==1) { echo "<p>No images found</p>"; } 
  ?>
  </div>
</body>
</html>
