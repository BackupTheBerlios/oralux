;; w3m
(setq w3m-key-binding 'info)
(load-library "w3m")
;; (setq w3m-home-page "http://oralux.org")
(setq w3m-use-cookies t) 
;; 

(setq w3m-content-type-alist (cons '("application/ogg" "\\.ogg\\'" ("ogg123" file)  nil) w3m-content-type-alist))
(setq w3m-content-type-alist (cons '("audio/mpeg" "\\.mp3\\'" ("mpg321" file) nil) w3m-content-type-alist))

