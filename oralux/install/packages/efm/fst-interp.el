;;; File: fst-interp.el
;;; Author: P.L. Nageoire
;;; Date: Jun 23 2003
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; An emacs lisp driver for the Festival speech synthesizer.
;;; This file is an equivalent for dtk-interp.el.
;;; The apropriate choice between dtk and whatever else will be
;;; done in the tts-setup-methods method in tts-setup library.

(eval-when-compile (require 'cl))
(declaim  (optimize  (safety 0) (speed 3)))
(require 'dtk-speak) ;; I didn't rename de speaker-process !
;;{{{ silence

(defsubst fst-interp-silence (duration force)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        (format "sh %d%s\n"
;;                                duration
;;                                (if force "\nd" ""))))
) ;; PLN 2003/04/02 not essential at the beginning.
;;}}}
;;{{{  tone

(defsubst fst-interp-tone (pitch duration &optional force)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        (format "t %d %d%s\n"
;;                                pitch duration
;;                                (if force "\nd" ""))))
) ;; PLN 2003/04/02 probably not implementable with
;; festival. Must be implemented externaly.
;;}}}
;;{{{ This method is specific to festival and
;;; its own speech command.
;;; PLN 2003/04/03.

(defsubst fst-quote-string ()
  " quote " )

( defvar fst-quote ?\" )

(defsubst fst-preparse-string ( string )
  (declare ( special fst-quote ))
  (setq fst-found-quote
	(position 
	 fst-quote
	 string ))
  (setq beg 0 )
  (setq res-string
	(substring
	 string
	 beg
	 fst-found-quote ))
  (while fst-found-quote
    (progn
      (setq res-string
	    (concat
	     res-string
	     (fst-quote-string)))
      (setq beg (+ fst-found-quote 1))
      (setq inter-pos
	    (position
	     fst-quote
	     (substring
	      string
	      beg )))
      (setq fst-found-quote
	    (if inter-pos
		(+
		 (+ inter-pos
		    fst-found-quote)
		 1)
	      nil))
      (setq res-string
	    (concat
	     res-string
	     (substring
	      string
	      beg
	      fst-found-quote)))))
  res-string)

(defsubst fst-format (string)
  "This method format a string to be spoken by
the festival speech synthesizer. The audio mode is
switched to async allowing further interruption of
the speech flu."
  ( format "(audio_mode 'async)\n(efm_speak_text \"%s\")\n"
	   ( fst-preparse-string string )))
;;}}}

(defsubst fst-interp-notes-initialize()
  (declare (special dtk-speaker-process))
;;  (process-send-string dtk-speaker-process "notes_initialize\n"))
) ;; PLN 2003/04/03 I don't understand the meaning of this
;; method.
(defsubst fst-interp-notes-shutdown()
  (declare (special dtk-speaker-process))
;;  (process-send-string dtk-speaker-process "notes_shutdown\n"))
) ;; PLN 2003/04/03 I don't understand the meaning of this
;; method.
(defsubst fst-interp-note (instrument pitch duration
                                      &optional target step force)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        (format "n %s %s %s %s %s%s\n"
;;                                instrument pitch duration
;;                                (or target 0)
;;                                (or step 5)
;;                                (if force "\nd" ""))))
) ;; PLN 2003/04/03 I don't know if it is
;; implementable by festival itself.
;;}}}
;;{{{  queue 

(defsubst fst-interp-queue (text)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
;;                       (format "q {%s }\n"
		       ( fst-format text)))
;; PLN 2003/04/03 No queue implemented at the moement.
;; I'm not shure that it's really necessary.

(defsubst fst-interp-queue-set-rate(rate)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
                       (format "(Parameter.set 'Duration_Stretch %s)\n" (+ 1.87 (* rate -0.00382)))))
;;                       (format "(Parameter.set 'Duration_Stretch %s)\n" rate)))

;;}}}
;;{{{  speak

(defsubst fst-interp-speak ()
  (declare (special dtk-speaker-process))
;;  (process-send-string dtk-speaker-process
;;                       "d\n"))
) ;; PLN 2003/04/03 Designed to flush the queue.
;; So we don't implement it at the moment because 
;; no queue is implemented.

;;}}}
;;{{{ say

(defsubst fst-interp-say (string)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
;;                       (format  "tts_say { %s}\n"
		       ( fst-format string )))

;;}}}
;;{{{ dispatch

;;;synonym for above in current server:
(defsubst fst-interp-dispatch (string)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
;;                       (format  "tts_say { %s}\n"
		       ( fst-format string )))

;;}}}
;;{{{ stop

(defsubst fst-interp-stop ()
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process 
;; "s\n" ))
		       "(audio_mode 'shutup)\n"))
;;}}}
;;{{{ sync

(defsubst fst-interp-sync()
  (declare (special dtk-speaker-process
                    fst-punctuation-mode fst-speech-rate 
                    fst-capitalize fst-split-caps
                    fst-allcaps-beep))
;;   (process-send-string dtk-speaker-process
;;                        (format "tts_sync_state %s %s %s %s %s \n"
;;                                fst-punctuation-mode 
;;                                (if fst-capitalize 1  0 )
;;                                (if fst-allcaps-beep 1  0 )
;;                                (if fst-split-caps 1 0 )
;;                                fst-speech-rate)))
) ;; PLN 2003/04/03 must probably require a piece of
;; festival program.
;;}}}
;;{{{  letter

(defsubst fst-interp-letter (letter)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
;;                       (format "l {%s}\n" 
		       ( fst-format letter )))

;;}}}
;;{{{  rate

(defsubst fst-interp-set-rate (rate)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process
                       (format 
;; "tts_set_speech_rate %s\n"
		       "(Parameter.set 'Duration_Stretch %s)\n"

		       (+ 1.87 (* rate -0.00382)))))
;;		       rate)))

;;}}}
;;{{{ character scale

(defsubst fst-interp-set-character-scale (factor)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        (format "tts_set_character_scale %s\n"
;;                                factor)))
) ;; PLN 2003/04/03 I don't know to what it
;; corresponds.
;;}}}
;;{{{  split caps 

(defsubst fst-interp-toggle-split-caps (fst-split-caps)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        (format "tts_split_caps %s\n"
;;                                (if fst-split-caps 1 0 ))))
) ;; PLN 2003/04/03 Festival doesn't have a
;; standard feature to do this.
;; If we want to implement it, we'll
;; probably need to write a piece of code.
;;}}}
;;{{{ capitalization

(defsubst fst-interp-toggle-capitalization (fst-capitalize)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process 
;;                        (format "tts_capitalize  %s\n"
;;                                (if fst-capitalize  1 0 ))))
) ;; PLN 2003/04/03 Just like the method above.
;;}}}
;;{{{ allcaps beep

(defsubst fst-interp-toggle-allcaps-beep  (fst-allcaps-beep)
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process 
;;                        (format "tts_allcaps_beep  %s\n"
;;                                (if fst-allcaps-beep  1 0
;;                                    ))))
) ;; PLN 2003/04/03 Just like the method above.
;;}}}
;;{{{ punctuations

(defsubst fst-interp-set-punctuations(mode)
  (declare (special dtk-speaker-process))
  (process-send-string dtk-speaker-process  
;;                        (format "tts_set_punctuations %s\n"
		       (format
			"(efm_set_punctuations \"%s\")\n"
			mode)))
;;; PLN 2003/06/26
;;}}}
;;{{{ reset

(defsubst fst-interp-reset-state ()
  (declare (special dtk-speaker-process))
;;  (process-send-string dtk-speaker-process "tts_reset \n"))
) ;; PLN 2003/04/03 No standard reset method in
;; festival so we'll need to implemented something.
;;}}}
;;{{{ pause

(defsubst fst-interp-pause ()
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        "tts_pause\n"))
) ;; PLN 2003/04/03 There's no pause method either in
;; festival must implement one if necessary.
;;}}}
;;{{{ resume

(defsubst fst-interp-resume ()
  (declare (special dtk-speaker-process))
;;   (process-send-string dtk-speaker-process
;;                        "tts_resume\n"))
) ;; PLN 2003/04/03 No standard resume method as well in
;; festival one must be implemented.
;;}}}

(provide 'fst-interp)
;;{{{  local variables

;;; local variables:
;;; folded-file: t
;;; byte-compile-dynamic: t
;;; byte-compile-dynamic: nil
;;; end: 

;;}}}
