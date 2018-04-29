package projtut;

import java.awt.BorderLayout;
import java.io.File;
import java.io.FileInputStream;

import javax.swing.JFrame;
import javax.swing.JPanel;

import com.adobe.acrobat.Viewer;

/*
 * www.codeurjava.com
 */
@SuppressWarnings("serial")
public class LecteurPDF extends JPanel{

    private Viewer viewer;
 
    public LecteurPDF(String nomfichier) throws Exception{
        this.setLayout(new BorderLayout());
 
        //créer le viewer qui va servir à afficher le contenu du pdf
        viewer = new Viewer();
        this.add(viewer, BorderLayout.CENTER);
        FileInputStream fis = new FileInputStream(new File(nomfichier));
        viewer.setDocumentInputStream(fis);
        viewer.activate();
    }
 
    public static void main(String[] args) throws Exception {
 
        String nomfichier = "tp7.pdf";
        LecteurPDF lecteur = new LecteurPDF(nomfichier);
        //créer le JFrame
        JFrame f = new JFrame("Lecteur PDF");
        f.setSize(1024,768);
        f.setLocationRelativeTo(null);
        f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        f.setVisible(true);
        f.getContentPane().add(lecteur);
    }
}