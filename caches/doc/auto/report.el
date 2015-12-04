(TeX-add-style-hook
 "report"
 (lambda ()
   (TeX-add-to-alist 'LaTeX-provided-class-options
                     '(("article" "12pt")))
   (TeX-add-to-alist 'LaTeX-provided-package-options
                     '(("inputenc" "utf8") ("geometry" "margin=1in")))
   (TeX-run-style-hooks
    "latex2e"
    "article"
    "art12"
    "inputenc"
    "geometry"
    "amsmath"
    "amsthm"
    "amssymb"
    "enumerate"
    "listings"
    "color"
    "courier")
   (TeX-add-symbols
    "N"
    "Z")
   (LaTeX-add-environments
    '("corollary" LaTeX-env-args ["argument"] 1)
    '("question" LaTeX-env-args ["argument"] 1)
    '("problem" LaTeX-env-args ["argument"] 1)
    '("exercise" LaTeX-env-args ["argument"] 1)
    '("lemma" LaTeX-env-args ["argument"] 1)
    '("theorem" LaTeX-env-args ["argument"] 1))))

